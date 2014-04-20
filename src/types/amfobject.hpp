#pragma once
#ifndef AMFOBJECT_HPP
#define AMFOBJECT_HPP

#include <functional>
#include <map>

#include "deserializationcontext.hpp"
#include "deserializer.hpp"

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

#include "utils/amfitemptr.hpp"
#include "utils/amfobjecttraits.hpp"

namespace amf {

class AmfObject : public AmfItem {
public:
	AmfObject() : traits("", false, false) { };
	AmfObject(std::string className, bool dynamic, bool externalizable) :
		traits(AmfObjectTraits(className, dynamic, externalizable)) { };

	bool operator==(const AmfItem& other) const {
		const AmfObject* p = dynamic_cast<const AmfObject*>(&other);

		if (p == nullptr)
			return false;

		if (traits != p->traits)
			return false;

		if (traits.dynamic && dynamicProperties != p->dynamicProperties)
			return false;

		// we don't need to check if other.sealedProperties contains the attribute,
		// as this is taken care of by the traits equality check
		for (const std::string& it : traits.attributes) {
			if (sealedProperties.at(it) != p->sealedProperties.at(it))
				return false;
		}

		if (traits.externalizable && externalizer(this) != p->externalizer(p))
			return false;

		return true;
	}

	std::vector<u8> serialize() const {
		/* AmfObject is defined as
		 * object-marker
		 * (
		 *   U29O-ref |
		 *   (U29O-traits-ext class-name *(U8)) |
		 *   U29O-traits-ref |
		 *   (U29O-traits class-name *(UTF-8-vr) *(value-type) *(dynamic-member))
		 * )
		 */

		std::vector<u8> buf = { AMF_OBJECT };

		AmfString className(traits.className);
		// serialized class name with AmfString marker
		std::vector<u8> name(className.serialize());

		// TODO: need to handle trait refs?
		if (traits.externalizable) {
			// U29O-traits-ext = 0b0111 = 0x07
			buf.push_back(0x07);
			// class-name
			buf.insert(buf.end(), name.begin() + 1, name.end());

			// externalized value = *(U8)
			// note: this may throw if externalizer is not properly initialized
			std::vector<u8> externalized(externalizer(this));
			buf.insert(buf.end(), externalized.begin(), externalized.end());
			return buf;
		}

		// U29-traits = 0b0011 = 0x03
		size_t traitMarker = traits.attributes.size() << 4 | 0x03;
		// dynamic marker = 0b1000 = 0x08
		if (traits.dynamic) traitMarker |= 0x08;

		std::vector<u8> marker(AmfInteger(traitMarker).serialize());
		buf.insert(buf.end(), marker.begin() + 1, marker.end());

		// class-name, skip the AmfString marker
		buf.insert(buf.end(), name.begin() + 1, name.end());

		// sealed property names = *(UTF-8-vr)
		for (const std::string& attribute : traits.attributes) {
			std::vector<u8> attr(AmfString(attribute).serialize());
			// skip AmfString marker
			buf.insert(buf.end(), attr.begin() + 1, attr.end());
		}

		// sealed property values = *(value-type)
		for (const std::string& attribute : traits.attributes) {
			auto s = sealedProperties.at(attribute)->serialize();
			buf.insert(buf.end(), s.begin(), s.end());
		}

		// only encode *(dynamic-member) (including the end marker) if the object
		// is actually dynamic
		if(traits.dynamic) {
			// dynamic-members = UTF-8-vr value-type
			for (const auto& it : dynamicProperties) {
				AmfString attribute(it.first);
				const std::vector<u8> name(attribute.serialize());

				// skip AmfString marker again
				buf.insert(buf.end(), name.begin() + 1, name.end());

				auto s = it.second->serialize();
				buf.insert(buf.end(), s.begin(), s.end());
			}

			// final dynamic member = UTF-8-empty
			buf.push_back(0x01);
		}

		return buf;
	}

	template<class T>
	void addSealedProperty(std::string name, const T& value) {
		auto* a = &traits.attributes;
		if (std::find(a->begin(), a->end(), name) == a->end())
			traits.attributes.push_back(name);

		sealedProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	void addDynamicProperty(std::string name, const T& value) {
		dynamicProperties[name] = AmfItemPtr(new T(value));
	}

	template<class T>
	T& getSealedProperty(std::string name) {
		const auto& it = std::find(traits.attributes.begin(),
			traits.attributes.end(), name);

		if (it == traits.attributes.end())
			throw std::out_of_range("AmfObject::getSealedProperty");

		return sealedProperties.at(name).as<T>();
	}

	template<class T>
	T& getDynamicProperty(std::string name) {
		return dynamicProperties.at(name).as<T>();
	}

	static AmfObject deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end || *it++ != AMF_OBJECT)
			throw std::invalid_argument("AmfObject: Invalid type marker");

		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0x00) {
			// 0b...0 == U29O-ref
			return ctx.getObject<AmfObject>(type >> 1);
		}

		AmfObjectTraits traits("", false, false);
		if ((type & 0x03) == 0x01) {
			// 0b..01 == U29O-traits-ref
			traits = ctx.getTraits(type >> 2);
		} else {
			if ((type & 0x07) == 0x07) {
				// 0b.111 == U29O-traits-ext
				traits.externalizable = true;
				traits.className = AmfString::deserializeValue(it, end, ctx);
			} else if ((type & 0x07) == 0x03) {
				// 0b.011 == U29O-traits
				traits.dynamic = ((type & 0x08) == 0x08);
				traits.className = AmfString::deserializeValue(it, end, ctx);
				int numSealed = type >> 4;
				for (int i = 0; i < numSealed; ++i)
					traits.attributes.push_back(AmfString::deserializeValue(it, end, ctx));
			}

			ctx.addTraits(traits);
		}

		AmfObject ret(traits);
		size_t contextIndex = ctx.addObject<AmfObject>(ret);

		if (traits.externalizable) {
			ret = Deserializer::externalDeserializers.at(traits.className)(it, end, ctx);
			ctx.setObject<AmfObject>(contextIndex, ret);
			return ret;
		}

		for (auto name : traits.attributes) {
			AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
			ret.sealedProperties[name] = val;
		}

		if (traits.dynamic) {
			while (true) {
				std::string name = AmfString::deserializeValue(it, end, ctx);
				if (name == "") break;

				AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
				ret.dynamicProperties[name] = val;
			}
		}

		ctx.setObject<AmfObject>(contextIndex, ret);
		return ret;
	}

	const AmfObjectTraits& objectTraits() const {
		return traits;
	}

	std::map<std::string, AmfItemPtr> sealedProperties;
	std::map<std::string, AmfItemPtr> dynamicProperties;

	std::function<v8(const AmfObject*)> externalizer;

private:
	AmfObject(AmfObjectTraits traits) : traits(traits) { };

	AmfObjectTraits traits;
};

} // namespace amf

#endif

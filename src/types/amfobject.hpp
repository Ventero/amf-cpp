#pragma once
#ifndef AMFOBJECT_HPP
#define AMFOBJECT_HPP

#include <functional>
#include <map>
#include <memory>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {

class AmfObject;
// TODO: nested class?
class AmfObjectTraits {
public:
	AmfObjectTraits(std::string className, bool dynamic, bool externalizable) :
		className(className), dynamic(dynamic), externalizable(externalizable) { };

	std::string className;
	std::vector<std::string> attributes;
	bool dynamic;
	bool externalizable;
};

class AmfObject : public AmfItem {
public:
	AmfObject() : traits(AmfObjectTraits("", false, false)) { };
	AmfObject(std::string className, bool dynamic, bool externalizable) :
		traits(AmfObjectTraits(className, dynamic, externalizable)) { };
	AmfObject(AmfObjectTraits traits) : traits(traits) { };

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
		int traitMarker = traits.attributes.size() << 4 | 0x03;
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
		traits.attributes.push_back(name);
		sealedProperties[name] = std::shared_ptr<AmfItem>(new T(value));
	}

	template<class T>
	void addDynamicProperty(std::string name, const T& value) {
		dynamicProperties[name] = std::shared_ptr<AmfItem>(new T(value));
	}

	template<class T>
	T& getSealedProperty(std::string name) {
		const auto& it = std::find(traits.attributes.begin(),
			traits.attributes.end(), name);

		if (it == traits.attributes.end())
			throw std::out_of_range("AmfObject::getSealedProperty");

		return *static_cast<T*>(sealedProperties.at(name).get());
	}

	template<class T>
	T& getDynamicProperty(std::string name) {
		return *static_cast<T*>(dynamicProperties.at(name).get());
	}

	std::map<std::string, std::shared_ptr<AmfItem>> sealedProperties;
	std::map<std::string, std::shared_ptr<AmfItem>> dynamicProperties;

	std::function<v8(const AmfObject*)> externalizer;

private:
	AmfObjectTraits traits;
};

} // namespace amf

#endif

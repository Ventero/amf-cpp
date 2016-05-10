#include "amfobject.hpp"

#include "deserializer.hpp"
#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {

bool AmfObject::operator==(const AmfItem& other) const {
	const AmfObject* p = dynamic_cast<const AmfObject*>(&other);

	if (p == nullptr)
		return false;

	if (traits != p->traits)
		return false;

	if (traits.dynamic && dynamicProperties != p->dynamicProperties)
		return false;

	// TODO: only compare properties that are in attributes?
	if (sealedProperties != p->sealedProperties)
		return false;

	// If this is an externalizable object, compare equal when they serialize
	// to the same data.
	if (traits.externalizable) {
		SerializationContext this_ctx, p_ctx;
		return (externalizer(this, this_ctx) == p->externalizer(p, p_ctx));
	}

	return true;
}

std::vector<u8> AmfObject::serialize(SerializationContext& ctx) const {
	/* AmfObject is defined as
	 * object-marker
	 * (
	 *   U29O-ref |
	 *   (U29O-traits-ext class-name *(U8)) |
	 *   U29O-traits-ref |
	 *   (U29O-traits class-name *(UTF-8-vr)
	 * )
	 * *(value-type) *(dynamic-member)
	 */
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_OBJECT, u8(index << 1) };
	ctx.addObject(*this);

	std::vector<u8> buf = { AMF_OBJECT };

	AmfString className(traits.className);
	// serialized class name as UTF-8-vr
	std::vector<u8> name(className.serializeValue(ctx));

	// ensure we do not serialize duplicate attribute names (see the
	// comment on traits.attributes in amfobjecttraits.hpp).
	std::set<std::string> attributes = traits.getUniqueAttributes();

	int trait_index = ctx.getIndex(traits);
	if (trait_index != -1) {
		// U29O-traits-ref = 0b..01
		buf.push_back(u8((trait_index << 2) | 1));
	} else {
		ctx.addTraits(traits);

		if (traits.externalizable) {
			// U29O-traits-ext = 0b0111 = 0x07
			buf.push_back(0x07);
			// class-name
			buf.insert(buf.end(), name.begin(), name.end());
		} else {
			// U29-traits = 0b0011 = 0x03
			size_t traitMarker = attributes.size() << 4 | 0x03;
			// dynamic marker = 0b1000 = 0x08
			if (traits.dynamic)
				traitMarker |= 0x08;

			std::vector<u8> marker(AmfInteger(traitMarker).serialize(ctx));
			buf.insert(buf.end(), marker.begin() + 1, marker.end());

			// class-name
			buf.insert(buf.end(), name.begin(), name.end());

			// sealed property names = *(UTF-8-vr)
			for (const std::string& attribute : attributes) {
				std::vector<u8> attr(AmfString(attribute).serializeValue(ctx));
				buf.insert(buf.end(), attr.begin(), attr.end());
			}
		}
	}

	if (traits.externalizable) {
		// externalized value = *(U8)
		// note: this may throw if externalizer is not properly initialized
		std::vector<u8> externalized(externalizer(this, ctx));
		buf.insert(buf.end(), externalized.begin(), externalized.end());
		return buf;
	}

	// sealed property values = *(value-type)
	for (const std::string& attribute : attributes) {
		auto s = sealedProperties.at(attribute)->serialize(ctx);
		buf.insert(buf.end(), s.begin(), s.end());
	}

	// only encode *(dynamic-member) (including the end marker) if the object
	// is actually dynamic
	if (traits.dynamic) {
		// dynamic-members = UTF-8-vr value-type
		for (const auto& it : dynamicProperties) {
			AmfString attribute(it.first);
			const std::vector<u8> name(attribute.serializeValue(ctx));
			buf.insert(buf.end(), name.begin(), name.end());

			auto s = it.second->serialize(ctx);
			buf.insert(buf.end(), s.begin(), s.end());
		}

		// final dynamic member = UTF-8-empty
		buf.push_back(0x01);
	}

	return buf;
}

AmfItemPtr AmfObject::deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_OBJECT)
		throw std::invalid_argument("AmfObject: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0x00) {
		// 0b...0 == U29O-ref
		return ctx.getPointer<AmfObject>(type >> 1);
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
			for (int i = 0; i < numSealed; ++i) {
				// Always add all attribute names, even if they're duplicates.
				// See the comment in amfobjecttraits.hpp
				traits.attributes.push_back(AmfString::deserializeValue(it, end, ctx));
			}
		}

		ctx.addTraits(traits);
	}

	AmfItemPtr ptr(new AmfObject(traits));
	AmfObject & ret = ptr.as<AmfObject>();
	ctx.addPointer(ptr);

	if (traits.externalizable) {
		ret = Deserializer::externalDeserializers.at(traits.className)(it, end, ctx);
		return ptr;
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

	return ptr;
}

AmfObject AmfObject::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	return deserializePtr(it, end, ctx).as<AmfObject>();
}

} // namespace amf

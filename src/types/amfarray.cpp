#include "amfarray.hpp"

#include "deserializer.hpp"
#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {

bool AmfArray::operator==(const AmfItem& other) const {
	const AmfArray* p = dynamic_cast<const AmfArray*>(&other);
	return p != nullptr && dense == p->dense && associative == p->associative;
}

std::vector<u8> AmfArray::serialize(SerializationContext& ctx) const {
	/*
	 * array-marker
	 * (
	 * 	U29O-ref |
	 * 	(U29A-value *(assoc-value) UTF-8-empty *(value-type))
	 * )
	 */

	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_ARRAY, u8(index << 1) };
	ctx.addObject(*this);

	// U29A-value
	std::vector<u8> buf = AmfInteger::asLength(dense.size(), AMF_ARRAY);

	// *(assoc-value) = (UTF-8-vr value-type)
	for (const auto& it : associative) {
		auto name = AmfString(it.first).serializeValue(ctx);
		auto value = it.second->serialize(ctx);

		// UTF-8-vr
		buf.insert(buf.end(), name.begin(), name.end());
		// value-type
		buf.insert(buf.end(), value.begin(), value.end());
	}

	// UTF-8-empty
	buf.push_back(0x01);

	// *(value-type)
	for (const auto& it : dense) {
		auto s = it->serialize(ctx);
		buf.insert(buf.end(), s.begin(), s.end());
	}

	return buf;
}

AmfItemPtr AmfArray::deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_ARRAY)
		throw std::invalid_argument("AmfArray: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getPointer<AmfArray>(type >> 1);

	// Create the return value and store it in the deserialization context.
	// By having the context point to the actual array we're constructing here
	// instead of a copy, we enable circular references.
	AmfItemPtr ret(new AmfArray());
	ctx.addPointer(ret);

	AmfArray & array = ret.as<AmfArray>();

	// associative until UTF-8-empty
	while (true) {
		std::string name = AmfString::deserializeValue(it, end, ctx);
		if (name == "") break;

		AmfItemPtr val = Deserializer::deserialize(it, end, ctx);
		array.associative[name] = val;
	}

	// dense
	int length = type >> 1;
	for (int i = 0; i < length; ++i)
		array.dense.push_back(Deserializer::deserialize(it, end, ctx));

	return ret;
}

AmfArray AmfArray::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	return deserializePtr(it, end, ctx).as<AmfArray>();
}

} // namespace amf

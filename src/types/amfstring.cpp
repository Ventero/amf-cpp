#include "amfstring.hpp"

#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"

namespace amf {

bool AmfString::operator==(const AmfItem& other) const {
	const AmfString* p = dynamic_cast<const AmfString*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfString::serialize(SerializationContext& ctx) const {
	// AmfString = string-marker UTF-8-vr
	std::vector<u8> buf { AMF_STRING };

	// Get the UTF-8-vr and append.
	std::vector<u8> value = serializeValue(ctx);
	buf.insert(buf.end(), value.begin(), value.end());

	return buf;
}

std::vector<u8> AmfString::serializeValue(SerializationContext& ctx) const {
	// UTF-8-empty should not be cached.
	if (value.empty())
		return std::vector<u8> { 0x01 };

	int index = ctx.getIndex(value);
	if (index != -1)
		return std::vector<u8> { u8(index << 1) };
	ctx.addString(value);

	// UTF-8-vr = U29S-value *(UTF8-char)
	// U29S-value encodes the length of the following string
	std::vector<u8> buf = AmfInteger::asLength(value.size(), AMF_STRING);

	// Get rid of the type marker.
	buf.erase(buf.begin());

	// now, append the actual string.
	buf.insert(buf.end(), value.begin(), value.end());

	return buf;
}

AmfString AmfString::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_STRING)
		throw std::invalid_argument("AmfString: Invalid type marker");

	return AmfString(deserializeValue(it, end, ctx));
}

std::string AmfString::deserializeValue(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getString(type >> 1);

	int length = type >> 1;
	if (end - it < length)
		throw std::out_of_range("Not enough bytes for AmfString");

	std::string val(it, it + length);
	it += length;

	ctx.addString(val);
	return val;
}

} // namespace amf

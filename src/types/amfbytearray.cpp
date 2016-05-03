#include "amfbytearray.hpp"

#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"

namespace amf {

bool AmfByteArray::operator==(const AmfItem& other) const {
	const AmfByteArray* p = dynamic_cast<const AmfByteArray*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfByteArray::serialize(SerializationContext& ctx) const {
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_BYTEARRAY, u8(index << 1) };
	ctx.addObject(*this);

	std::vector<u8> buf = AmfInteger::asLength(value.size(), AMF_BYTEARRAY);
	buf.insert(buf.end(), value.begin(), value.end());

	return buf;
}

AmfByteArray AmfByteArray::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_BYTEARRAY)
		throw std::invalid_argument("AmfByteArray: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getObject<AmfByteArray>(type >> 1);

	int length = type >> 1;
	if (end - it < length)
		throw std::out_of_range("Not enough bytes for AmfByteArray");

	AmfByteArray ret(it, it + length);
	it += length;

	ctx.addObject<AmfByteArray>(ret);

	return ret;
}

} // namespace amf

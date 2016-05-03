#include "amfxml.hpp"

#include "serializationcontext.hpp"
#include "types/amfinteger.hpp"

namespace amf {

bool AmfXml::operator==(const AmfItem& other) const {
	const AmfXml* p = dynamic_cast<const AmfXml*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfXml::serialize(SerializationContext& ctx) const {
	int index = ctx.getIndex(*this);
	if (index != -1)
		return std::vector<u8> { AMF_XML, u8(index << 1) };
	ctx.addObject(*this);

	std::vector<u8> buf = AmfInteger::asLength(value.size(), AMF_XML);

	// the actual data is simply encoded as UTF8-chars
	buf.insert(buf.end(), value.begin(), value.end());

	return buf;
}

AmfXml AmfXml::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end || *it++ != AMF_XML)
		throw std::invalid_argument("AmfXml: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getObject<AmfXml>(type >> 1);

	int length = type >> 1;
	if (end - it < length)
		throw std::out_of_range("Not enough bytes for AmfXml");

	std::string val(it, it + length);
	it += length;

	AmfXml ret(val);
	ctx.addObject<AmfXml>(ret);

	return ret;
}

} // namespace amf

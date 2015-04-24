#include "amfxmldocument.hpp"

#include "deserializationcontext.hpp"
#include "types/amfinteger.hpp"
#include "types/amfxml.hpp"

namespace amf {

bool AmfXmlDocument::operator==(const AmfItem& other) const {
	const AmfXmlDocument* p = dynamic_cast<const AmfXmlDocument*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfXmlDocument::serialize() const {
	// XMLDocument is identical to XML (except for the object marker),
	// so simply forward everything to the AmfXml implementation
	std::vector<u8> buf = AmfXml(value).serialize();
	buf[0] = AMF_XMLDOC;

	return buf;
}

AmfXmlDocument AmfXmlDocument::deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
	if (it == end || *it++ != AMF_XMLDOC)
		throw std::invalid_argument("AmfXmlDocument: Invalid type marker");

	int type = AmfInteger::deserializeValue(it, end);
	if ((type & 0x01) == 0)
		return ctx.getObject<AmfXmlDocument>(type >> 1);

	int length = type >> 1;
	if (end - it < length)
		throw std::out_of_range("Not enough bytes for AmfXmlDocument");

	std::string val(it, it + length);
	it += length;

	AmfXmlDocument ret(val);
	ctx.addObject<AmfXmlDocument>(ret);

	return ret;
}

} // end namespace amf

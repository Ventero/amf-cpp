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
	// Encode the type marker + length.
	std::vector<u8> buf = AmfInteger::asLength(value.size(), AMF_XMLDOC);

	// Encode the data. According to the spec it's encoded as UTF-8 chars.
	// We leave it up to the caller to ensure that's the case.
	buf.insert(buf.end(), value.begin(), value.end());

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

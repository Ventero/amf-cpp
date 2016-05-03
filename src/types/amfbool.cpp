#include "amfbool.hpp"

namespace amf
{

bool AmfBool::operator==(const AmfItem& other) const {
	const AmfBool* p = dynamic_cast<const AmfBool*>(&other);
	return p != nullptr && value == p->value;
}

AmfBool AmfBool::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&) {
	if (it == end)
		throw std::invalid_argument("AmfBool: End of iterator");

	u8 marker = *it++;
	if (marker != AMF_TRUE && marker != AMF_FALSE)
		throw std::invalid_argument("AmfBool: Invalid type marker");

	return AmfBool(marker == AMF_TRUE);
}

} // namespace amf

#include "deserializer.hpp"

#include "types/amfitem.hpp"

#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfdate.hpp"
#include "types/amfdictionary.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"
#include "types/amfvector.hpp"
#include "types/amfxml.hpp"
#include "types/amfxmldocument.hpp"

namespace amf {

std::map<std::string, ExternalDeserializerFunction> Deserializer::externalDeserializers({ });

AmfItemPtr Deserializer::deserialize(v8 data, SerializationContext& ctx) {
	auto it = data.cbegin();
	return deserialize(it, data.cend(), ctx);
}

AmfItemPtr Deserializer::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
	if (it == end)
		throw std::out_of_range("Deserializer::deserialize end of input");

	u8 type = *it;
	switch (type) {
		case AMF_UNDEFINED:
			return AmfItemPtr(AmfUndefined::deserialize(it, end, ctx));
		case AMF_NULL:
			return AmfItemPtr(AmfNull::deserialize(it, end, ctx));
		case AMF_FALSE:
		case AMF_TRUE:
			return AmfItemPtr(AmfBool::deserialize(it, end, ctx));
		case AMF_INTEGER:
			return AmfItemPtr(AmfInteger::deserialize(it, end, ctx));
		case AMF_DOUBLE:
			return AmfItemPtr(AmfDouble::deserialize(it, end, ctx));
		case AMF_STRING:
			return AmfItemPtr(AmfString::deserialize(it, end, ctx));
		case AMF_XMLDOC:
			return AmfItemPtr(AmfXmlDocument::deserialize(it, end, ctx));
		case AMF_DATE:
			return AmfItemPtr(AmfDate::deserialize(it, end, ctx));
		case AMF_ARRAY:
			return AmfArray::deserializePtr(it, end, ctx);
		case AMF_OBJECT:
			return AmfObject::deserializePtr(it, end, ctx);
		case AMF_XML:
			return AmfItemPtr(AmfXml::deserialize(it, end, ctx));
		case AMF_BYTEARRAY:
			return AmfItemPtr(AmfByteArray::deserialize(it, end, ctx));
		case AMF_VECTOR_INT:
			return AmfItemPtr(AmfVector<int>::deserialize(it, end, ctx));
		case AMF_VECTOR_UINT:
			return AmfItemPtr(AmfVector<unsigned int>::deserialize(it, end, ctx));
		case AMF_VECTOR_DOUBLE:
			return AmfItemPtr(AmfVector<double>::deserialize(it, end, ctx));
		case AMF_VECTOR_OBJECT:
			return AmfVector<AmfItem>::deserializePtr(it, end, ctx);
		case AMF_DICTIONARY:
			return AmfDictionary::deserializePtr(it, end, ctx);
		default:
			throw std::invalid_argument("Deserializer::deserialize: Invalid type byte");
	}
}

AmfItemPtr Deserializer::deserialize(v8 buf) {
	auto it = buf.cbegin();
	return deserialize(it, buf.cend(), ctx);
}

} // namespace amf

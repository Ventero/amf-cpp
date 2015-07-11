#include "serializer.hpp"

#include "types/amfitem.hpp"

namespace amf {

Serializer& Serializer::operator<<(const AmfItem& item) {
	std::vector<u8> serialized = item.serialize(ctx);
	buf.insert(buf.end(), serialized.begin(), serialized.end());

	return *this;
}

} // namespace amf

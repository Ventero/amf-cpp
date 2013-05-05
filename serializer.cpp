#include "serializer.hpp"

Serializer& Serializer::operator<<(const AmfItem& item) {
	std::vector<u8> serialized = item.serialize();
	buf.insert(buf.end(), serialized.begin(), serialized.end());

	return *this;
}

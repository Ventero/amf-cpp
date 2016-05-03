#include "amfdouble.hpp"

namespace amf {

bool AmfDouble::operator==(const AmfItem& other) const {
	const AmfDouble* p = dynamic_cast<const AmfDouble*>(&other);
	return p != nullptr && value == p->value;
}

std::vector<u8> AmfDouble::serialize(SerializationContext&) const {
	std::vector<u8> buf = { AMF_DOUBLE };

	v8 bytes = network_bytes(value);
	buf.insert(buf.end(), bytes.begin(), bytes.end());
	return buf;
}

AmfDouble AmfDouble::deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&) {
	if (it == end || *it++ != AMF_DOUBLE)
		throw std::invalid_argument("AmfDouble: Invalid type marker");

	// Dates are always encoded as 64bit double in network order.
	if (end - it < 8)
		throw std::out_of_range("Not enough bytes for AmfDouble");

	double v;
	std::copy(it, it + 8, reinterpret_cast<u8 *>(&v));
	it += 8;

	return AmfDouble(ntoh(v));
}

} // namespace amf

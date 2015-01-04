#pragma once
#ifndef AMFDOUBLE_HPP
#define AMFDOUBLE_HPP

#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"

namespace amf {

class AmfDouble : public AmfItem {
public:
	AmfDouble() : value(0) { }
	AmfDouble(double v) : value(v) { }
	operator double() const { return value; }

	bool operator==(const AmfItem& other) const {
		const AmfDouble* p = dynamic_cast<const AmfDouble*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = { AMF_DOUBLE };

		v8 bytes = network_bytes(value);
		buf.insert(buf.end(), bytes.begin(), bytes.end());
		return buf;
	}

	static AmfDouble deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext&) {
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

	double value;
};

} // namespace amf

#endif

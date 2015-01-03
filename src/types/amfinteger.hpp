#pragma once
#ifndef AMFINTEGER_HPP
#define AMFINTEGER_HPP

#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"
#include "types/amfdouble.hpp"

namespace amf {

class AmfInteger : public AmfItem {
public:
	AmfInteger() : value(0) { };
	AmfInteger(int v) : value(v) { };
	operator int() const { return value; }

	bool operator==(const AmfItem& other) const {
		const AmfInteger* p = dynamic_cast<const AmfInteger*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		// According to the spec:
		// If the value of an unsigned integer (uint) or signed integer (int)
		// is greater than or equal to 2^28, or if a signed integer (int) is
		// less than -2^28, it will be serialized using the AMF 3 double type
		if (value < -0x10000000 || value >= 0x10000000)
			return AmfDouble(value).serialize();

		if (value >= 0 && value <= 0x7F) {
			return std::vector<u8> {
				AMF_INTEGER,
				u8(value)
			};
		} else if (value > 0x7F && value <= 0x3FFF) {
			return std::vector<u8> {
				AMF_INTEGER,
				u8(value >> 7 | 0x80),
				u8(value & 0x7F)
			};
		} else if (value > 0x3FFF && value <= 0x1FFFFF) {
			return std::vector<u8> {
				AMF_INTEGER,
				u8(value >> 14 | 0x80),
				u8(((value >> 7) & 0x7F) | 0x80),
				u8(value & 0x7F)
			};
		} else {
			return std::vector<u8> {
				AMF_INTEGER,
				u8(value >> 22 | 0x80),
				u8(((value >> 15) & 0x7F) | 0x80),
				u8(((value >> 8 ) & 0x7F) | 0x80),
				u8(value & 0xFF)
			};
		}
	}

	static std::vector<u8> asLength(size_t value, u8 marker) {
		std::vector<u8> buf = AmfInteger(value << 1 | 1).serialize();
		buf[0] = marker;

		return buf;
	}

	static AmfInteger deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext&) {
		if (it == end || *it++ != AMF_INTEGER)
			throw std::invalid_argument("AmfInteger: Invalid type marker");

		return AmfInteger(deserializeValue(it, end));
	}

	static int deserializeValue(v8::const_iterator& it, v8::const_iterator end) {
		// Byte counter
		int i = 0;
		// Integer value, limited to 29 bits.
		int val = 0;

		if (it == end)
			throw std::out_of_range("Not enough bytes for AmfInteger");

		// up to 3 bytes with high bit set for values > 255
		while (i++ < 3 && *it & 0x80) {
			val <<= 7;
			val |= (*it++ & 0x7F);

			if (it == end)
				throw std::out_of_range("Not enough bytes for AmfInteger");
		}

		// last byte
		val <<= (i <= 3 ? 7 : 8);
		val |= *it++;

		// set sign bit to handle negative integers
		val <<= 3;
		val >>= 3;

		return val;
	}

	int value;
};

} // namespace amf

#endif

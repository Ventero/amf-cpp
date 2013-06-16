#pragma once
#ifndef AMFINTEGER_HPP
#define AMFINTEGER_HPP

#include "amfitem.hpp"
#include "amfdouble.hpp"

class AmfInteger : public AmfItem {
public:
	AmfInteger() : value(0) { };
	AmfInteger(int v) : value(v) { };
	operator int() const { return value; }

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

	std::vector<u8> asLength(u8 marker) {
		value = value << 1 | 1;

		std::vector<u8> buf = serialize();
		buf[0] = marker;

		return buf;
	}

private:
	int value;
};

#endif

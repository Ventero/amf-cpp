#pragma once
#ifndef AMFINTEGER_HPP
#define AMFINTEGER_HPP

#include "amfitem.hpp"

class AmfInteger : public AmfItem {
public:
	AmfInteger(int v) : value(v) { };
	operator int() const { return value; }

	std::vector<u8> serialize() const {
		// According to the spec:
		// If the value of an unsigned integer (uint) or signed integer (int)
		// is greater than or equal to 2^28, or if a signed integer (int) is
		// less than -2^28, it will be serialized using the AMF 3 double type
		if (value < -0x10000000 || value >= 10000000)
			return AmfDouble(value).serialize();

		// TODO: range checking here? if out of range, serialize as double or throw?
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

private:
	int value;
};

#endif

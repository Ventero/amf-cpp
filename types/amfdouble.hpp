#pragma once
#ifndef AMFDOUBLE_HPP
#define AMFDOUBLE_HPP

#include "types/amfitem.hpp"

namespace amf {

class AmfDouble : public AmfItem {
public:
	AmfDouble() : value(0) { };
	AmfDouble(double v) : value(v) { };
	operator double() const { return value; }

	std::vector<u8> serialize() const {
		std::vector<u8> buf = { AMF_DOUBLE };

		double netvalue = hton(value);
		const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
		buf.insert(buf.end(), bytes, bytes + sizeof(double));
		return buf;
	}

private:
	double value;
};

} // namespace amf

#endif

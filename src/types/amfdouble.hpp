#pragma once
#ifndef AMFDOUBLE_HPP
#define AMFDOUBLE_HPP

#include "deserializationcontext.hpp"
#include "types/amfitem.hpp"

namespace amf {

class AmfDouble : public AmfItem {
public:
	AmfDouble() : value(0) { };
	AmfDouble(double v) : value(v) { };
	operator double() const { return value; }

	std::vector<u8> serialize() const {
		std::vector<u8> buf = { AMF_DOUBLE };

		v8 bytes = network_bytes(value);
		buf.insert(buf.end(), bytes.begin(), bytes.end());
		return buf;
	}

	template<typename Iter>
	static AmfDouble deserialize(Iter& it, Iter end, DeserializationContext&) {
		v8 data(it, end);

		if(data.size() < 8)
			throw std::out_of_range("Not enough bytes for AmfDouble");

		it += 8;

		double v = *reinterpret_cast<double*>(&data[0]);
		return AmfDouble(ntoh(v));
	}

	double value;
};

} // namespace amf

#endif

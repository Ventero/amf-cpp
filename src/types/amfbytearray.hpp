#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

namespace amf {

class AmfByteArray : public AmfItem {
public:
	template<typename T>
	AmfByteArray(const T& v) {
		using std::begin;
		using std::end;
		value = std::vector<u8>(begin(v), end(v));
	}

	template<typename T>
	AmfByteArray(T begin, T end) {
		value = std::vector<u8>(begin, end);
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(value.size()).asLength(AMF_BYTEARRAY);

		buf.insert(buf.end(), value.begin(), value.end());
		return buf;
	}

	template<typename Iter>
	static AmfByteArray deserialize(Iter& it, Iter end) {
		int type = AmfInteger::deserialize(it, end);
		if ((type & 0x01) == 0)
			throw std::invalid_argument("Object references not yet implemented");

		int length = type >> 1;
		if (end - it < length)
			throw std::out_of_range("Not enough bytes for AmfByteArray");

		AmfByteArray ret(it, it + length);
		it += length;

		return ret;
	}

	std::vector<u8> value;
};

} // namespace amf

#endif

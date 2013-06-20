#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "amfitem.hpp"
#include "amfinteger.hpp"

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

private:
	std::vector<u8> value;
};

#endif

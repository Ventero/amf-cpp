#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "amfitem.hpp"

class AmfByteArray : public AmfItem {
public:
	template<typename T>
	AmfByteArray(const T& v) {
		using std::begin;
		using std::end;
		value = std::vector<u8>(begin(v), end(v));
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf {
			AMF_BYTEARRAY,
			u8(value.size() << 1 | 1)
		};
		buf.insert(buf.end(), value.begin(), value.end());
		return buf;
	}

private:
	std::vector<u8> value;
};

#endif

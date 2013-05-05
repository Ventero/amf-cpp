#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "amfitem.hpp"

class AmfByteArray : public AmfItem {
public:
	AmfByteArray(std::vector<u8> v) : value(v) { };

	std::vector<u8> serialize() const override {
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

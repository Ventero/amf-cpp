#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

class AmfString : public AmfItem {
public:
	AmfString(std::string v) : value(v) { };
	operator std::string() const { return value; }

	std::vector<u8> serialize() const {
		if(value.empty())
			return std::vector<u8>{ AMF_STRING, 0x01 };

		// AmfString = string-marker UTF-8-vr
		// with UTF-8-vr = U29S-value *(UTF8-char)
		// U29S-value encodes the length of the following string
		std::vector<u8> buf = AmfInteger(value.size()).asLength(AMF_STRING);

		// now, append the actual string.
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string value;
};

#endif

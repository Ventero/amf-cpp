#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>
#include "amfitem.hpp"
#include "amfinteger.hpp"

class AmfString : public AmfItem {
public:
	AmfString(std::string v) : value(v) { };
	operator std::string() const { return value; }

	std::vector<u8> serialize() const {
		if(value.empty())
			return std::vector<u8>{ AMF_STRING, 0x01 };

		// strings are encoded as AMF_STRING, followed by their length and
		// the string encoded as UTF8. thus, we start by serializing the
		// length and overwriting the type marker of the serialized data
		// with an AMF_STRING marker.
		// length << 1 | 1 to mark string as non-reference
		AmfInteger length(value.length() << 1 | 1);
		auto buf = length.serialize();
		buf[0] = AMF_STRING;

		// now, append the actual string.
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string value;
};

#endif

#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>

#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

namespace amf {

class AmfString : public AmfItem {
public:
	AmfString() { }
	AmfString(const char* v) : value(v) { }
	AmfString(std::string v) : value(v) { }
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

	template<typename Iter>
	static AmfString deserialize(Iter& it, Iter end, DeserializationContext& ctx) {
		int type = AmfInteger::deserialize(it, end, ctx).value;
		if ((type & 0x01) == 0)
			return ctx.getString(type >> 1);

		int length = type >> 1;
		if (end - it < length)
			throw std::out_of_range("Not enough bytes for AmfString");

		std::string val(it, it + length);
		it += length;

		AmfString ret(val);
		ctx.addString(ret);

		return ret;
	}

	std::string value;
};

} // namespace amf

#endif

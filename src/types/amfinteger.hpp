#pragma once
#ifndef AMFINTEGER_HPP
#define AMFINTEGER_HPP

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfInteger : public AmfItem {
public:
	AmfInteger() : value(0) { }
	AmfInteger(int v) : value(v) { }
	operator int() const { return value; }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext&) const;
	static std::vector<u8> asLength(size_t value, u8 marker);
	static AmfInteger deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&);
	static int deserializeValue(v8::const_iterator& it, v8::const_iterator end);

	int value;
};

} // namespace amf

#endif

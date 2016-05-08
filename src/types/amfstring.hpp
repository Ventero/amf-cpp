#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfString : public AmfItem {
public:
	AmfString() { }
	AmfString(const char* v) : value(v == nullptr ? "" : v) { }
	AmfString(std::string v) : value(v) { }
	operator std::string() const { return value; }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	std::vector<u8> serializeValue(SerializationContext& ctx) const;
	static AmfString deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);
	static std::string deserializeValue(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::string value;
};

} // namespace amf

#endif

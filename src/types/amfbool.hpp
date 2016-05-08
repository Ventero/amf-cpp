#pragma once
#ifndef AMFBOOL_HPP
#define AMFBOOL_HPP

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfBool : public AmfItem {
public:
	AmfBool(bool v) : value(v) { }
	operator bool() const { return value; }

	bool operator==(const AmfItem& other) const;

	std::vector<u8> serialize(SerializationContext&) const {
		return std::vector<u8>{ value ? AMF_TRUE : AMF_FALSE };
	}

	static AmfBool deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&);

	bool value;
};

} // namespace amf

#endif

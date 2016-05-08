#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfByteArray : public AmfItem {
public:
	AmfByteArray() { }
	AmfByteArray(const AmfByteArray& other) : value(other.value) { }

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

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfByteArray deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::vector<u8> value;
};

} // namespace amf

#endif

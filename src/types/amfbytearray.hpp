#pragma once
#ifndef AMFBYTEARRAY_HPP
#define AMFBYTEARRAY_HPP

#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"

namespace amf {

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

	bool operator==(const AmfItem& other) const {
		const AmfByteArray* p = dynamic_cast<const AmfByteArray*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		std::vector<u8> buf = AmfInteger(value.size()).asLength(AMF_BYTEARRAY);

		buf.insert(buf.end(), value.begin(), value.end());
		return buf;
	}

	static AmfByteArray deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end || *it++ != AMF_BYTEARRAY)
			throw std::invalid_argument("AmfByteArray: Invalid type marker");

		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0)
			return ctx.getObject<AmfByteArray>(type >> 1);

		int length = type >> 1;
		if (end - it < length)
			throw std::out_of_range("Not enough bytes for AmfByteArray");

		AmfByteArray ret(it, it + length);
		it += length;

		ctx.addObject<AmfByteArray>(ret);

		return ret;
	}

	std::vector<u8> value;
};

} // namespace amf

#endif

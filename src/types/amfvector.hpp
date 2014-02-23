#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <vector>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

namespace amf {

template<typename T>
struct VectorMarker;

template<>
struct VectorMarker<int> {
	static const u8 value = AMF_VECTOR_INT;
	typedef void type;
};

template<>
struct VectorMarker<unsigned int> {
	static const u8 value = AMF_VECTOR_UINT;
	typedef void type;
};

template<>
struct VectorMarker<double> {
	static const u8 value = AMF_VECTOR_DOUBLE;
	typedef void type;
};

template<typename T, class Enable = void>
class AmfVector;

template<typename T>
class AmfVector<T, typename VectorMarker<T>::type> : public AmfItem {
public:
	AmfVector() : values({}), fixed(false) { };
	AmfVector(std::vector<T> vector, bool fixed = false) :
		values(vector), fixed(fixed) { };

	void push_back(T item) {
		values.push_back(item);
	}

	std::vector<u8> serialize() const {
		// U29V value
		std::vector<u8> buf = AmfInteger(values.size()).asLength(VectorMarker<T>::value);

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		for(const T& it : values) {
			// values are encoded as in network byte order
			// ints are encoded as U32, not U29
			T netvalue = hton(it);
			const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
			buf.insert(buf.end(), bytes, bytes + sizeof(T));
		}

		return buf;
	}
private:
	std::vector<T> values;
	bool fixed;
};

template<typename T>
class AmfVector<T, typename std::enable_if<std::
	is_base_of<AmfItem, T>::value>::type> : public AmfItem {
public:
	AmfVector(std::vector<T> vector, std::string type, bool fixed = false) :
		type(type), fixed(fixed) {
		for (const auto& it : vector)
			push_back(it);
	};

	void push_back(const AmfItem& item) {
		values.push_back(item.serialize());
	}

	std::vector<u8> serialize() const {
		// U29V value, encoding the length
		std::vector<u8> buf = AmfInteger(values.size()).asLength(AMF_VECTOR_OBJECT);

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		// object type name
		std::vector<u8> typeName = AmfString(type).serialize();
		// skip the AMF_STRING marker
		buf.insert(buf.end(), typeName.begin() + 1, typeName.end());

		for (const auto& it : values) {
			buf.insert(buf.end(), it.begin(), it.end());
		}

		return buf;
	}

private:
	std::vector<std::vector<u8>> values;
	std::string type;
	bool fixed;
};

} // namespace amf

#endif

#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <vector>
#include "amfitem.hpp"
#include "amfinteger.hpp"

template<typename T>
struct VectorMarker;

template<>
struct VectorMarker<int> {
	static const u8 value = AMF_VECTOR_INT;
};

template<>
struct VectorMarker<unsigned int> {
	static const u8 value = AMF_VECTOR_UINT;
};

template<>
struct VectorMarker<double> {
	static const u8 value = AMF_VECTOR_DOUBLE;
};

template<typename T, class Enable = void>
class AmfVector;

template<typename T>
class AmfVector<T, typename std::enable_if<std::is_constructible<
	VectorMarker<T>>::value>::type> : public AmfItem, private std::vector<T> {
public:
	AmfVector(std::vector<T> vector, bool fixed = false) :
		std::vector<T>(vector), fixed(fixed) { };

	using std::vector<T>::begin;
	using std::vector<T>::end;
	using std::vector<T>::insert;
	using std::vector<T>::push_back;

	std::vector<u8> serialize() const {
		// U29V value
		AmfInteger length(this->size() << 1 | 1);
		std::vector<u8> buf = length.serialize();
		// overwrite the int marker with the correct vector one
		buf[0] = VectorMarker<T>::value;

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		for(T it : *this) {
			// values are encoded as in network byte order
			// ints are encoded as U32, not U29
			T netvalue = hton(it);
			const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
			buf.insert(buf.end(), bytes, bytes + sizeof(T));
		}

		return buf;
	}
private:
	bool fixed;
};

#endif

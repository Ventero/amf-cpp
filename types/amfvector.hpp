#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <vector>
#include "amfitem.hpp"
#include "amfinteger.hpp"
#include "amfstring.hpp"

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
class AmfVector<T, typename std::enable_if<
	std::is_class<VectorMarker<T>>::value>::type> : public AmfItem {
public:
	AmfVector(std::vector<T> vector, bool fixed = false) :
		vector(vector), fixed(fixed) { };

	std::vector<u8> serialize() const {
		// U29V value
		AmfInteger length(vector.size() << 1 | 1);
		std::vector<u8> buf = length.serialize();
		// overwrite the int marker with the correct vector one
		buf[0] = VectorMarker<T>::value;

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		for(T it : vector) {
			// values are encoded as in network byte order
			// ints are encoded as U32, not U29
			T netvalue = hton(it);
			const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
			buf.insert(buf.end(), bytes, bytes + sizeof(T));
		}

		return buf;
	}
private:
	std::vector<T> vector;
	bool fixed;
};

#endif

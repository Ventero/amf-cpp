#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <vector>
#include "amfitem.hpp"
#include "amfinteger.hpp"

template<typename T>
class AmfVector;

template<>
class AmfVector<int> : public AmfItem {
public:
	AmfVector(std::vector<int> vector, bool fixed = false) :
		vector(vector), fixed(fixed) { };

	std::vector<u8> serialize() const {
		// U29V value
		AmfInteger length(vector.size() << 1 | 1);
		std::vector<u8> buf = length.serialize();
		// overwrite the int marker with the correct vector one
		buf[0] = AMF_VECTOR_INT;

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		// TODO: FIXME
		static_assert(sizeof(int) == 4, "int cannot be encoded as U32");
		for(int it : vector) {
			// value is encoded as U32 in network byte order, not AmfInt (U29)
			int netvalue = hton(it);
			const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
			buf.insert(buf.end(), bytes, bytes + 4);
		}

		return buf;
	}
private:
	std::vector<int> vector;
	bool fixed;
};

#endif

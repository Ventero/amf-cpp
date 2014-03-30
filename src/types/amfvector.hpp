#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <vector>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

#include "utils/amfitemptr.hpp"

namespace amf {

template<typename T>
struct VectorProperties;

template<>
struct VectorProperties<int> {
	static const u8 marker = AMF_VECTOR_INT;
	static const unsigned int size = 4;
	typedef void type;
};

template<>
struct VectorProperties<unsigned int> {
	static const u8 marker = AMF_VECTOR_UINT;
	static const unsigned int size = 4;
	typedef void type;
};

template<>
struct VectorProperties<double> {
	static const u8 marker = AMF_VECTOR_DOUBLE;
	static const unsigned int size = 8;
	typedef void type;
};

template<typename T, class Enable = void>
class AmfVector;

template<typename T>
class AmfVector<T, typename VectorProperties<T>::type> : public AmfItem {
public:
	AmfVector() : values({}), fixed(false) { };
	AmfVector(std::vector<T> vector, bool fixed = false) :
		values(vector), fixed(fixed) { };

	bool operator==(const AmfItem& other) const {
		const AmfVector<T>* p = dynamic_cast<const AmfVector<T>*>(&other);
		return p != nullptr && fixed == p->fixed && values == p->values;
	}

	void push_back(T item) {
		values.push_back(item);
	}

	T& at(int index) {
		return values.at(index);
	}

	std::vector<u8> serialize() const {
		// U29V value
		std::vector<u8> buf = AmfInteger(values.size()).asLength(
			VectorProperties<T>::marker);

		// fixed-vector marker
		buf.push_back(fixed ? 0x01 : 0x00);

		for(const T& it : values) {
			// values are encoded in network byte order
			// ints are encoded as U32, not U29
			T netvalue = hton(it);
			const u8* bytes = reinterpret_cast<const u8*>(&netvalue);
			buf.insert(buf.end(), bytes, bytes + VectorProperties<T>::size);
		}

		return buf;
	}

	static AmfVector<T> deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) {
		if (it == end)
			throw std::invalid_argument("AmfVector: End of iterator");

		u8 marker = *it++;
		if (marker != VectorProperties<T>::marker)
			throw std::invalid_argument("AmfVector: Invalid type marker");

		int type = AmfInteger::deserializeValue(it, end);
		if ((type & 0x01) == 0)
			return ctx.getObject<AmfVector<T>>(type >> 1);

		size_t count = type >> 1;

		if(it == end)
			throw std::out_of_range("Not enough bytes for AmfVector");
		bool fixed = (*it++ == 0x01);

		v8 data(it, end);
		if(data.size() < count * VectorProperties<T>::size)
			throw std::out_of_range("Not enough bytes for AmfVector");

		it += count * VectorProperties<T>::size;

		std::vector<T> values(count);
		for (size_t i = 0; i < count; ++i) {
			values[i] = ntoh(*reinterpret_cast<T*>(&data[i * VectorProperties<T>::size]));
		}

		AmfVector<T> ret(values, fixed);
		ctx.addObject<AmfVector<T>>(ret);

		return ret;
	}

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

	bool operator==(const AmfItem& other) const {
		const AmfVector<T>* p = dynamic_cast<const AmfVector<T>*>(&other);
		return p != nullptr && fixed == p->fixed && type == p->type && values == p->values;
	}

	void push_back(const T& item) {
		values.emplace_back(new T(item));
	}

	T& at(int index) {
		return *static_cast<T*>(values.at(index).get());
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
			auto s = it->serialize();
			buf.insert(buf.end(), s.begin(), s.end());
		}

		return buf;
	}

	std::vector<AmfItemPtr> values;
	std::string type;
	bool fixed;
};

} // namespace amf

#endif

#pragma once
#ifndef AMFVECTOR_HPP
#define AMFVECTOR_HPP

#include <string>
#include <vector>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"

namespace amf {

class SerializationContext;

template<typename T>
struct VectorProperties;

template<typename T, class Enable = void>
class AmfVector;

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

template<typename T>
class AmfVector<T, typename VectorProperties<T>::type> : public AmfItem {
public:
	AmfVector() : values({}), fixed(false) { }
	AmfVector(std::vector<T> vector, bool fixed = false) :
		values(vector), fixed(fixed) { }

	bool operator==(const AmfItem& other) const;

	void push_back(T item) {
		values.push_back(item);
	}

	T& at(int index) {
		return values.at(index);
	}

	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfVector<T> deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::vector<T> values;
	bool fixed;
};

template<>
class AmfVector<AmfItem> : public AmfItem {
public:
	AmfVector(std::string type, bool fixed = false) : type(type), fixed(fixed) { }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfItemPtr deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);
	static AmfVector<AmfItem> deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	template<typename V, typename std::enable_if<std::is_base_of<AmfItem, V>::value, int>::type = 0>
	AmfVector<V> as() {
		AmfVector<V> ret({}, type, fixed);
		ret.values = values;
		return ret;
	}

	std::vector<AmfItemPtr> values;
	std::string type;
	bool fixed;
};

template<typename T>
class AmfVector<T, typename std::enable_if<
	std::is_base_of<AmfItem, T>::value>::type> : public AmfVector<AmfItem> {
public:
	AmfVector(std::vector<T> vector, std::string type, bool fixed = false) :
		AmfVector<AmfItem>(type, fixed) {
		for (const auto& it : vector)
			push_back(it);
	}

	bool operator==(const AmfItem& other) const {
		const AmfVector<T>* p = dynamic_cast<const AmfVector<T>*>(&other);
		return p != nullptr && fixed == p->fixed && type == p->type && values == p->values;
	}

	void push_back(const T& item) {
		values.emplace_back(new T(item));
	}

	T& at(int index) {
		return values.at(index).template as<T>();
	}

	static AmfVector<T> deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx) {
		return AmfVector<AmfItem>::deserialize(it, end, ctx).as<T>();
	}

private:
	static AmfItemPtr deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);
};

} // namespace amf

#endif

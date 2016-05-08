#pragma once
#ifndef AMFDICTIONARY_HPP
#define AMFDICTIONARY_HPP

#include <unordered_map>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"

namespace amf {

class SerializationContext;

struct AmfDictionaryHash {
	size_t operator()(const AmfItemPtr& val) const;
};

class AmfDictionary : public AmfItem {
public:
	AmfDictionary(bool numbersAsStrings, bool weak = false) :
		asString(numbersAsStrings), weak(weak) { }

	bool operator==(const AmfItem& other) const;

	template<class T, class V>
	void insert(const T& key, const V& value) {
		static_assert(std::is_base_of<AmfItem, V>::value, "Values must extend AmfItem");

		(*this)[key].reset(new V(value));
	}

	template<class T, class V>
	T& at(const V& key) {
		auto ptr = (*this)[key];
		return ptr.template as<T>();
	}

	void clear() {
		values.clear();
	}

	std::vector<u8> serialize(SerializationContext & ctx) const;
	static AmfItemPtr deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);
	static AmfDictionary deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	bool asString;
	bool weak;
	std::unordered_map<
		AmfItemPtr,
		AmfItemPtr,
		AmfDictionaryHash
	> values;

private:
	template<class T>
	AmfItemPtr& operator[](const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Keys must extend AmfItem");
		return values[AmfItemPtr(new T(item))];
	}

	// Flash Player doesn't support deserializing booleans and number types
	// (AmfInteger/AmfDouble), so we may have to serialize them as strings
	v8 serializeKey(const AmfItemPtr& key, SerializationContext& ctx) const;
};

} // namespace amf

#endif

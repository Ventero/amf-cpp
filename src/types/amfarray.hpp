#pragma once
#ifndef AMFARRAY_HPP
#define AMFARRAY_HPP

#include <map>
#include <string>

#include "types/amfitem.hpp"
#include "utils/amfitemptr.hpp"

namespace amf {

class SerializationContext;

class AmfArray : public AmfItem {
public:
	AmfArray() { }

	template<class V>
	AmfArray(std::vector<V> densePart) {
		for (const V& it : densePart)
			push_back(it);
	}

	template<class V, class A>
	AmfArray(std::vector<V> densePart, std::map<std::string, A> associativePart) {
		for (const V& it : densePart)
			push_back(it);

		for (const auto& it : associativePart)
			insert(it.first, it.second);
	}

	template<class T>
	void push_back(const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Elements must extend AmfItem");

		dense.emplace_back(new T(item));
	}

	template<class T>
	void insert(const std::string key, const T& item) {
		static_assert(std::is_base_of<AmfItem, T>::value, "Elements must extend AmfItem");

		associative[key] = AmfItemPtr(new T(item));
	}

	template<class T>
	T& at(int index) {
		return dense.at(index).as<T>();
	}

	template<class T>
	const T& at(int index) const {
		return dense.at(index).as<T>();
	}

	template<class T>
	T& at(std::string key) {
		return associative.at(key).as<T>();
	}

	template<class T>
	const T& at(std::string key) const {
		return associative.at(key).as<T>();
	}

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfItemPtr deserializePtr(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);
	static AmfArray deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::vector<AmfItemPtr> dense;
	std::map<std::string, AmfItemPtr> associative;
};

} // namespace amf

#endif

#pragma once
#ifndef AMFARRAY_HPP
#define AMFARRAY_HPP

#include <map>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

#include "utils/amfitemptr.hpp"

namespace amf {

class AmfArray : public AmfItem {
public:
	AmfArray() { };

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

	bool operator==(const AmfItem& other) const {
		const AmfArray* p = dynamic_cast<const AmfArray*>(&other);
		return p != nullptr && dense == p->dense && associative == p->associative;
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
		return *static_cast<T*>(dense.at(index).get());
	}

	template<class T>
	T& at(std::string key) {
		return *static_cast<T*>(associative.at(key).get());
	}

	std::vector<u8> serialize() const {
		/*
		 * array-marker
		 * (
		 * 	U29O-ref |
		 * 	(U29A-value *(assoc-value) UTF-8-empty *(value-type))
		 * )
		 */
		// U29A-value
		std::vector<u8> buf = AmfInteger(dense.size()).asLength(AMF_ARRAY);

		for (const auto& it : associative) {
			auto name = AmfString(it.first).serialize();
			auto s = it.second->serialize();

			// skip AmfString marker
			buf.insert(buf.end(), name.begin() + 1, name.end());
			buf.insert(buf.end(), s.begin(), s.end());
		}

		// UTF-8-empty
		buf.push_back(0x01);

		for (const auto& it : dense) {
			auto s = it->serialize();
			buf.insert(buf.end(), s.begin(), s.end());
		}

		return buf;
	}

	std::vector<AmfItemPtr> dense;
	std::map<std::string, AmfItemPtr> associative;
};

} // namespace amf

#endif

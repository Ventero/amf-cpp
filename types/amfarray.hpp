#pragma once
#ifndef AMFARRAY_HPP
#define AMFARRAY_HPP

#include <map>

#include "types/amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

class AmfArray : public AmfItem {
public:
	AmfArray() { };

	AmfArray(std::vector<std::vector<u8>> densePart,
		std::map<std::string, std::vector<u8>> associativePart) :
		dense(densePart), associative(associativePart) { };

	template<class V>
	AmfArray(std::vector<V> densePart) {
		for (const auto& it : densePart)
			push_back(it);
	}

	template<class V, class A>
	AmfArray(std::vector<V> densePart,
		std::map<std::string, A> associativePart) {
		for (const auto& it : densePart)
			push_back(it);

		for (const auto& it : associativePart)
			insert(it.first, it.second);
	}

	void push_back(const AmfItem& item) {
		dense.push_back(item.serialize());
	}

	void insert(const std::string key, const AmfItem& item) {
		associative[key] = item.serialize();
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
			AmfString attribute(it.first);
			std::vector<u8> name(attribute.serialize());

			// skip AmfString marker
			buf.insert(buf.end(), name.begin() + 1, name.end());
			buf.insert(buf.end(), it.second.begin(), it.second.end());
		}

		// UTF-8-empty
		buf.push_back(0x01);

		for (const auto& it : dense) {
			buf.insert(buf.end(), it.begin(), it.end());
		}

		return buf;
	}

private:
	std::vector<std::vector<u8>> dense;
	std::map<std::string, std::vector<u8>> associative;

};
#endif

#pragma once
#ifndef AMFARRAY_HPP
#define AMFARRAY_HPP

#include <map>

#include "amfitem.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"

class AmfArray : public AmfItem {
public:
	AmfArray() { };

	AmfArray(std::vector<AmfItem*> densePart,
		std::map<std::string, AmfItem*> associativePart) :
		dense(densePart), associative(associativePart) { };

	std::vector<u8> serialize() const {
		/*
		 * array-marker
		 * (
		 * 	U29O-ref |
		 * 	(U29A-value *(assoc-value) UTF-8-empty *(value-type))
		 * )
		 */
		// U29A-value
		AmfInteger length(dense.size() << 1 | 1);
		std::vector<u8> buf = length.serialize();
		// write the correct type marker
		buf[0] = AMF_ARRAY;

		for (const auto& it : associative) {
			AmfString attribute(it.first);
			std::vector<u8> name(attribute.serialize());
			std::vector<u8> value(it.second->serialize());

			// skip AmfString marker
			buf.insert(buf.end(), name.begin() + 1, name.end());
			buf.insert(buf.end(), value.begin(), value.end());
		}

		// UTF-8-empty
		buf.push_back(0x01);

		for (const auto& it : dense) {
			std::vector<u8> value(it->serialize());
			buf.insert(buf.end(), value.begin(), value.end());
		}

		return buf;
	}

private:
	std::vector<AmfItem*> dense;
	std::map<std::string, AmfItem*> associative;

};
#endif

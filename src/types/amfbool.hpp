#pragma once
#ifndef AMFBOOL_HPP
#define AMFBOOL_HPP

#include <vector>
#include "types/amfitem.hpp"

namespace amf {

class AmfBool : public AmfItem {
public:
	AmfBool(bool v) : value(v) { }
	operator bool() const { return value; }

	bool operator==(const AmfItem& other) const {
		const AmfBool* p = dynamic_cast<const AmfBool*>(&other);
		return p != nullptr && value == p->value;
	}

	std::vector<u8> serialize() const {
		return std::vector<u8>{ value ? AMF_TRUE : AMF_FALSE };
	}

	static AmfBool deserialize(v8::const_iterator& it, v8::const_iterator end, DeserializationContext&) {
		if (it == end)
			throw std::invalid_argument("AmfBool: End of iterator");

		u8 marker = *it++;
		if (marker != AMF_TRUE && marker != AMF_FALSE)
			throw std::invalid_argument("AmfBool: Invalid type marker");

		return AmfBool(marker == AMF_TRUE);
	}

	bool value;
};

} // namespace amf

#endif

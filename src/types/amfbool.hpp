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

	bool value;
};

} // namespace amf

#endif

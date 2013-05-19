#pragma once
#ifndef AMFBOOL_HPP
#define AMFBOOL_HPP

#include <vector>
#include "amfitem.hpp"

class AmfBool : public AmfItem {
public:
	AmfBool(bool v) : value(v) { }
	operator bool() const { return value; }

	std::vector<u8> serialize() const {
		return std::vector<u8>{ value ? AMF_TRUE : AMF_FALSE };
	}

private:
	bool value;
};

#endif

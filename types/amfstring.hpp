#pragma once
#ifndef AMFSTRING_HPP
#define AMFSTRING_HPP

#include <string>
#include "amfitem.hpp"

class AmfString : public AmfItem {
public:
	AmfString(std::string v) : value(v) { };
	operator std::string() const { return value; }

	std::vector<u8> serialize() const {
		return std::vector<u8>{
			AMF_STRING
		};
	}

private:
	std::string value;
};

#endif

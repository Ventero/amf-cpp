#pragma once
#ifndef AMFUNDEFINED_HPP
#define AMFUNDEFINED_HPP

#include <vector>
#include "amfitem.hpp"

class AmfUndefined : public AmfItem {
public:
	AmfUndefined() {}

	std::vector<u8> serialize() const {
		return std::vector<u8>{ AMF_UNDEFINED };
	}
};

#endif

#pragma once
#ifndef AMFNULL_HPP
#define AMFNULL_HPP

#include <vector>
#include "amfitem.hpp"

class AmfNull : public AmfItem {
public:
	AmfNull() { }

	std::vector<u8> serialize() const override {
		return std::vector<u8>{ AMF_NULL };
	}
};

#endif

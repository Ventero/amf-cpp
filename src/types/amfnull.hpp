#pragma once
#ifndef AMFNULL_HPP
#define AMFNULL_HPP

#include <vector>

#include "types/amfitem.hpp"

namespace amf {

class AmfNull : public AmfItem {
public:
	AmfNull() { }

	std::vector<u8> serialize() const {
		return std::vector<u8>{ AMF_NULL };
	}
};

} // namespace amf

#endif

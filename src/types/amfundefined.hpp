#pragma once
#ifndef AMFUNDEFINED_HPP
#define AMFUNDEFINED_HPP

#include <vector>

#include "types/amfitem.hpp"

namespace amf {

class AmfUndefined : public AmfItem {
public:
	AmfUndefined() {}

	bool operator==(const AmfItem& other) const {
		const AmfUndefined* p = dynamic_cast<const AmfUndefined*>(&other);
		return p != nullptr;
	}

	std::vector<u8> serialize() const {
		return std::vector<u8>{ AMF_UNDEFINED };
	}
};

} // namespace amf

#endif

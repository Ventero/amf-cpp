#pragma once
#ifndef AMFUNDEFINED_HPP
#define AMFUNDEFINED_HPP

#include <vector>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfUndefined : public AmfItem {
public:
	AmfUndefined() {}

	bool operator==(const AmfItem& other) const {
		const AmfUndefined* p = dynamic_cast<const AmfUndefined*>(&other);
		return p != nullptr;
	}

	std::vector<u8> serialize(SerializationContext&) const {
		return std::vector<u8>{ AMF_UNDEFINED };
	}

	static AmfUndefined deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&) {
		if (it == end || *it++ != AMF_UNDEFINED)
			throw std::invalid_argument("AmfUndefined: Invalid type marker");

		return AmfUndefined();
	}

};

} // namespace amf

#endif

#pragma once
#ifndef AMFNULL_HPP
#define AMFNULL_HPP

#include <vector>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfNull : public AmfItem {
public:
	AmfNull() { }

	bool operator==(const AmfItem& other) const {
		const AmfNull* p = dynamic_cast<const AmfNull*>(&other);
		return p != nullptr;
	}

	std::vector<u8> serialize(SerializationContext&) const {
		return std::vector<u8>{ AMF_NULL };
	}

	static AmfNull deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext&) {
		if (it == end || *it++ != AMF_NULL)
			throw std::invalid_argument("AmfNull: Invalid type marker");

		return AmfNull();
	}

};

} // namespace amf

#endif

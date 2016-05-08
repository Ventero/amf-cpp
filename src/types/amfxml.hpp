#pragma once
#ifndef AMFXML_HPP
#define AMFXML_HPP

#include <string>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfXml : public AmfItem {
public:
	AmfXml() { }
	AmfXml(std::string value) : value(value) { }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfXml deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::string value;
};

} // namespace amf

#endif

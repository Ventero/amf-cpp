#pragma once
#ifndef AMFXMLDOCUMENT_HPP
#define AMFXMLDOCUMENT_HPP

#include <string>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

class AmfXmlDocument : public AmfItem {
public:
	AmfXmlDocument() { }
	AmfXmlDocument(std::string value) : value(value) { }

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfXmlDocument deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	std::string value;
};

} // namespace amf

#endif

#pragma once
#ifndef AMFXMLDOCUMENT_HPP
#define AMFXMLDOCUMENT_HPP

#include "types/amfitem.hpp"
#include "types/amfxml.hpp"

class AmfXmlDocument : public AmfItem {
public:
	AmfXmlDocument() { };
	AmfXmlDocument(std::string value) : value(value) { };

	std::vector<u8> serialize() const {
		// XMLDocument is identical to XML (except for the object marker),
		// so simply forward everything to the AmfXml implementation
		std::vector<u8> buf = AmfXml(value).serialize();
		buf[0] = AMF_XMLDOC;

		return buf;
	}

private:
	std::string value;
};

#endif

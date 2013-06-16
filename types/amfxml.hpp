#pragma once
#ifndef AMFXML_HPP
#define AMFXML_HPP

#include <string>
#include "amfitem.hpp"
#include "amfinteger.hpp"

class AmfXml : public AmfItem {
public:
	AmfXml() { };
	AmfXml(std::string value) : value(value) { };

	std::vector<u8> serialize() const {
		AmfInteger length(value.size() << 1 | 1);
		std::vector<u8> buf = length.serialize();
		// overwrite the int marker with the XML marker
		buf[0] = AMF_XML;

		// the actual data is simply encoded as UTF8-chars
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string value;
};

#endif

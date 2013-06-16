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
		std::vector<u8> buf = AmfInteger(value.size()).asLength(AMF_XML);

		// the actual data is simply encoded as UTF8-chars
		buf.insert(buf.end(), value.begin(), value.end());

		return buf;
	}

private:
	std::string value;
};

#endif

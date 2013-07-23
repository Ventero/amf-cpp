#pragma once
#ifndef AMFITEM_HPP
#define AMFITEM_HPP

#include <vector>
#include "amf.hpp"

enum AmfMarker : u8 {
	AMF_UNDEFINED,
	AMF_NULL,
	AMF_FALSE,
	AMF_TRUE,
	AMF_INTEGER,
	AMF_DOUBLE,
	AMF_STRING,
	AMF_XMLDOC,
	AMF_DATE,
	AMF_ARRAY,
	AMF_OBJECT,        // partially supported
	AMF_XML,
	AMF_BYTEARRAY,
	AMF_VECTOR_INT,
	AMF_VECTOR_UINT,
	AMF_VECTOR_DOUBLE,
	AMF_VECTOR_OBJECT,
	AMF_DICTIONARY
};

class AmfItem {
public:

	virtual std::vector<u8> serialize() const = 0;
};

#endif

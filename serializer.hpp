#pragma once
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <streambuf>
#include <vector>

#include "amf.hpp"
#include "types/amfitem.hpp"

#include "types/amfbool.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"

class Serializer {
public:
	Serializer() {};
	~Serializer() {};

	Serializer& operator<<(const AmfItem& item);

	const std::vector<u8> data() const { return buf; }
	void clear() { buf.clear(); }

private:
	std::vector<u8> buf;
};

#endif

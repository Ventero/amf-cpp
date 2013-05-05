#pragma once
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <streambuf>
#include <vector>

#include "amf.hpp"
#include "types/amfitem.hpp"

class Serializer {
public:
	Serializer() {};
	~Serializer() {};

	Serializer& operator<<(const AmfItem& item);

	std::vector<u8> const data() { return buf; }

private:
	std::vector<u8> buf;
};

#endif

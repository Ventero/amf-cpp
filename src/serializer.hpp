#pragma once
#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <vector>

#include "amf.hpp"

namespace amf {

class AmfItem;

class Serializer {
public:
	Serializer() { }
	~Serializer() { }

	Serializer& operator<<(const AmfItem& item);

	const std::vector<u8> & data() const { return buf; }
	void clear() { buf.clear(); }

private:
	std::vector<u8> buf;
};

} // namespace amf

#endif

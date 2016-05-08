#pragma once
#ifndef AMFDATE_HPP
#define AMFDATE_HPP

#include <chrono>
#include <ctime>

#include "types/amfitem.hpp"

namespace amf {

class SerializationContext;

// this needs to be a long long to ensure no overflow
namespace {
	static const long long MSEC_PER_SEC = 1000ll;
}

class AmfDate : public AmfItem {
public:
	// millisconds since epoch
	AmfDate(long long date) : value(date) { }
	AmfDate(std::tm* date) : value(mktime(date) * MSEC_PER_SEC) { }
	AmfDate(std::chrono::system_clock::time_point date);

	bool operator==(const AmfItem& other) const;
	std::vector<u8> serialize(SerializationContext& ctx) const;
	static AmfDate deserialize(v8::const_iterator& it, v8::const_iterator end, SerializationContext& ctx);

	long long value;
};

} // namespace amf

#endif

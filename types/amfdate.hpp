#pragma once
#ifndef AMFDATE_HPP
#define AMFDATE_HPP

#include <chrono>
#include <ctime>

#include "types/amfdouble.hpp"
#include "types/amfitem.hpp"

// this needs to be a long long to ensure no overflow
static const long long MSEC_PER_SEC = 1000ll;

class AmfDate : public AmfItem {
public:
	// for simplicity, we assume std::time_t is the seconds since epoch
	// this is at least guaranteed by POSIX
	AmfDate(long long date) : value(date) { };
	AmfDate(std::time_t date) : value(date * MSEC_PER_SEC) { };
	AmfDate(std::tm* date) : AmfDate(mktime(date)) { };

	AmfDate(std::chrono::system_clock::time_point date) {
		auto duration = date.time_since_epoch();
		value = std::chrono::duration_cast<std::chrono::milliseconds>(
			duration).count();
	}

	std::vector<u8> serialize() const {
		// AmfDate is date-marker (U29O-ref | (U29D-value date-time)),
		// where U29D-value is 1 and date-time is a int64 encoded as double
		std::vector<u8> buf { AMF_DATE, 0x01 };

		std::vector<u8> date(AmfDouble(value).serialize());
		// skip the AMF_DOUBLE marker
		buf.insert(buf.end(), date.begin() + 1, date.end());

		return buf;
	}

private:
	long long value;
};

#endif

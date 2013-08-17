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
	// millisconds since epoch
	AmfDate(long long date) : value(date) { };
	AmfDate(std::tm* date) : value(mktime(date) * MSEC_PER_SEC) { };

	AmfDate(std::chrono::system_clock::time_point date) {
		auto duration = date.time_since_epoch();
		value = std::chrono::duration_cast<std::chrono::milliseconds>(
			duration).count();
	}

	std::vector<u8> serialize() const {
		// AmfDate is date-marker (U29O-ref | (U29D-value date-time)),
		// where U29D-value is 1 and date-time is a int64 describing the number of
		// milliseconds since epoch, encoded as double
		std::vector<u8> buf { AMF_DATE, 0x01 };

		// dates are serialised as double, ignoring the precision loss
		std::vector<u8> date(AmfDouble(static_cast<double>(value)).serialize());
		// skip the AMF_DOUBLE marker
		buf.insert(buf.end(), date.begin() + 1, date.end());

		return buf;
	}

private:
	long long value;
};

#endif

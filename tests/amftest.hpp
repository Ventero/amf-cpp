#include <vector>

#include "gtest/gtest.h"
#include "amf.hpp"
#include "types/amfitem.hpp"

using namespace amf;

static inline void isEqual(const std::vector<u8>& expected, const v8& serialized) {
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();
}

static inline void isEqual(const std::vector<u8>& expected, const AmfItem& value) {
	v8 serialized = value.serialize();
	isEqual(expected, serialized);
}

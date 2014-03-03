#include <vector>

#include "gtest/gtest.h"
#include "amf.hpp"
#include "types/amfitem.hpp"

using namespace amf;

static inline void isEqual(const std::vector<u8>& expected, const v8& serialized) {
	ASSERT_EQ(expected, serialized)
		<< "Expected length " << expected.size()
		<< ", got " << serialized.size();
}

static inline void isEqual(const std::vector<u8>& expected, const AmfItem& value) {
	v8 serialized = value.serialize();
	isEqual(expected, serialized);
}

template<typename T, typename V>
void deserializesTo(V expected, const v8& data, int expectedLeft = 0) {
	auto it = data.begin();
	T i = T::deserialize(it, data.end());
	ASSERT_EQ(expectedLeft, data.end() - it)
		<< "Expected " << expectedLeft
		<< " bytes left, got " << (data.end() - it)
		<< " bytes left";
	ASSERT_EQ(expected, i.value)
		<< "Expected value " << ::testing::PrintToString(expected)
		<< ", got " << ::testing::PrintToString(i.value);
}

#include "amftest.hpp"

#include "gtest/gtest.h"

#include "amf.hpp"
#include "types/amfnull.hpp"

TEST(NullSerializationTest, SimpleValue) {
	ASSERT_EQ(v8 { 0x01 }, AmfNull().serialize());
}

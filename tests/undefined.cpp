#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfbool.hpp"
#include "types/amfnull.hpp"
#include "types/amfundefined.hpp"

TEST(UndefinedSerializationTest, SimpleValue) {
	ASSERT_EQ(v8 { 0x00 }, AmfUndefined().serialize());
}

TEST(UndefinedEquality, SimpleValue) {
	AmfUndefined u1, u2;
	EXPECT_EQ(u1, u2);
}

TEST(UndefinedEquality, MixedType) {
	AmfUndefined u;
	AmfNull n;
	EXPECT_NE(u, n);

	AmfBool b(false);
	EXPECT_NE(u, b);
}

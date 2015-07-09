#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfbool.hpp"
#include "types/amfnull.hpp"
#include "types/amfundefined.hpp"

TEST(UndefinedSerializationTest, SimpleValue) {
	isEqual(v8 { 0x00 }, AmfUndefined());
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

TEST(UndefinedDeserialization, SimpleValue) {
	deserialize(AmfUndefined(), v8 { 0x00, }, 0);
	deserialize(AmfUndefined(), v8 { 0x00, 0x00 }, 1);
	deserialize(AmfUndefined(), v8 { 0x00, 0x00, 0x00, 0x00 }, 3);
}

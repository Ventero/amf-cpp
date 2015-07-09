#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfbool.hpp"
#include "types/amfnull.hpp"
#include "types/amfundefined.hpp"

TEST(NullSerializationTest, SimpleValue) {
	isEqual(v8 { 0x01 }, AmfNull());
}

TEST(NullEquality, SimpleValue) {
	AmfNull n1, n2;
	EXPECT_EQ(n1, n2);
}

TEST(NullEquality, MixedType) {
	AmfNull n;
	AmfUndefined u;
	EXPECT_NE(n, u);

	AmfBool b(false);
	EXPECT_NE(n, b);
}

TEST(NullDeserialization, SimpleValue) {
	deserialize(AmfNull(), v8 { 0x01, }, 0);
	deserialize(AmfNull(), v8 { 0x01, 0x00 }, 1);
	deserialize(AmfNull(), v8 { 0x01, AMF_NULL }, 1);
	deserialize(AmfNull(), v8 { 0x01, 0x00, 0x00, 0x00 }, 3);
}

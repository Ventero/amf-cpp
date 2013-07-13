#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfdate.hpp"

TEST(DateSerializationTest, FromLongLong) {
	AmfDate date(136969002755210ll);
	v8 expected { 0x08, 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49, 0x22, 0x80 };
	ASSERT_EQ(expected, date.serialize());
}

TEST(DateSerializationTest, FromTm) {
	std::time_t time = 1234567890;
	AmfDate date(localtime(&time));
	v8 expected { 0x08, 0x01, 0x42, 0x71, 0xf7, 0x1f, 0xb0, 0x45, 0x00, 0x00 };
	ASSERT_EQ(expected, date.serialize());
}

TEST(DateSerializationTest, FromTimePoint) {
	std::chrono::seconds s(1500000000);
	std::chrono::system_clock::time_point time(s);
	AmfDate date(time);
	v8 expected { 0x08, 0x01, 0x42, 0x75, 0xd3, 0xef, 0x79, 0x80, 0x00, 0x00 };
	ASSERT_EQ(expected, date.serialize());
}

TEST(DateSerializationTest, Epoch) {
	std::chrono::system_clock::time_point time;
	AmfDate date(time);
	v8 expected { 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	ASSERT_EQ(expected, date.serialize());
}

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
	std::tm res;
#ifdef _WIN32
	localtime_s(&res, &time);
#else
	localtime_r(&time, &res);
#endif
	AmfDate date(&res);
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

static void deserializesTo(long long expected, const v8& data, int left = 0,
	DeserializationContext* ctx = nullptr) {
	deserialize<AmfDate>(expected, data, left, ctx);
}

TEST(DateDeserializationTest, Values) {
	deserializesTo(136969002755210ll, { 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49,
		0x22, 0x80 });
	deserializesTo(1234567890000ll, { 0x01, 0x42, 0x71, 0xf7, 0x1f, 0xb0, 0x45,
		0x00, 0x00 });
	deserializesTo(1500000000000ll, { 0x01, 0x42, 0x75, 0xd3, 0xef, 0x79, 0x80,
		0x00, 0x00 });
	deserializesTo(136969002755210ll, { 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49,
		0x22, 0x80, 0x90 }, 1);
}

TEST(DateDeserializationTest, ObjectCache) {
	DeserializationContext ctx;
	deserializesTo(136969002755210ll, { 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49, 0x22, 0x80 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x00 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49, 0x22, 0x80 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x00 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x02 }, 0, &ctx);
	deserializesTo(1234567890000ll,   { 0x01, 0x42, 0x71, 0xf7, 0x1f, 0xb0, 0x45, 0x00, 0x00 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x00 }, 0, &ctx);
	deserializesTo(136969002755210ll, { 0x02 }, 0, &ctx);
	deserializesTo(1234567890000ll,   { 0x04 }, 0, &ctx);
}

TEST(DateDeserializationTest, NotEnoughBytes) {
	v8 data = { 0x01, 0x42, 0xdf, 0x24, 0xa5, 0x30, 0x49, 0x22 };
	auto it = data.begin();
	DeserializationContext ctx;
	ASSERT_THROW(AmfDate::deserialize(it, data.end(), ctx), std::out_of_range);
}

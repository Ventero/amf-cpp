#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfbool.hpp"

static void isEqual(const std::vector<u8>& expected, bool value) {
	isEqual(expected, AmfBool(value));
}

TEST(BoolSerializationTest, SimpleValues) {
	isEqual(v8 { 0x02 }, false);
	isEqual(v8 { 0x03 }, true);
}

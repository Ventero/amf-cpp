#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfinteger.hpp"
#include "types/amfstring.hpp"
#include "types/amfobject.hpp"
#include "types/amfxml.hpp"
#include "types/amfxmldocument.hpp"

static void isEqual(const std::vector<u8>& expected, const char* value) {
	isEqual(expected, AmfString(value));
}

TEST(AmfString, NullptrCtor) {
	ASSERT_NO_THROW(AmfString(nullptr));
}

TEST(StringSerialization, EmptyString) {
	isEqual(v8 { 0x06, 0x01 }, "");
}

TEST(StringSerialization, Ascii) {
	isEqual(v8 { 0x06, 0x07, 0x62, 0x61, 0x72 }, "bar");
	isEqual(v8 { 0x06, 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 }, "boofar");
	isEqual(v8 { 0x06, 0x07, 0x71, 0x75, 0x78 }, "qux");
	isEqual(v8 { 0x06, 0x07, 0x71, 0x75, 0x7A }, "quz");
	isEqual(v8 { 0x06, 0x09, 0x71, 0x75, 0x75, 0x78 }, "quux");
	isEqual(v8 { 0x06, 0x07, 0x22, 0x27, 0x5C }, "\"'\\");
}

TEST(StringSerialization, MultiByteLength) {
	isEqual(v8 { 0x06, 0x84, 0x59, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		           0x61, 0x61, 0x61, 0x61, 0x61, 0x61 }, std::string(300, 'a').c_str());
}

TEST(StringSerialization, Umlaute) {
	isEqual(v8 { 0x06, 0x17, 0xC3, 0xBC, 0x6D, 0x6C, 0xC3, 0xA4, 0xC3, 0xBC, 0x74,
		           0xC3, 0xAB }, "ümläütë");
}

TEST(StringSerialization, Unicode) {
	isEqual(v8 { 0x06, 0x29, 0xC4, 0xA7, 0xC4, 0xB8, 0xC3, 0xB0, 0x40, 0xC3, 0xBE,
		           0xC3, 0xA6, 0xC4, 0xB8, 0xC5, 0xBF, 0xE2, 0x80, 0x9C, 0xC3, 0xB0
		         }, "ħĸð@þæĸſ“ð");
	isEqual(v8 { 0x06, 0x59, 0xE2, 0x80, 0x9D, 0x5D, 0xC2, 0xB2, 0xC2, 0xB3, 0xC2,
		           0xB6, 0xC5, 0xA7, 0xE2, 0x86, 0x93, 0xC3, 0xB8, 0xC4, 0xA7, 0xE2,
		           0x80, 0x9D, 0xE2, 0x80, 0x9C, 0xC5, 0x82, 0xC2, 0xB5, 0xC3, 0xA6,
		           0xC3, 0xB0, 0xC2, 0xB5, 0xE2, 0x86, 0x92, 0xC2, 0xB3, 0xC3, 0xB8,
		           0xC4, 0xA7 }, "”]²³¶ŧ↓øħ”“łµæðµ→³øħ");
}

TEST(StringSerialization, SerializeValue) {
	SerializationContext ctx;
	isEqual(v8 { 0x07, 0x62, 0x61, 0x72 }, AmfString("bar").serializeValue(ctx));
	isEqual(v8 { 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 }, AmfString("boofar").serializeValue(ctx));
	isEqual(v8 { 0x07, 0x71, 0x75, 0x78 }, AmfString("qux").serializeValue(ctx));
	isEqual(v8 { 0x07, 0x71, 0x75, 0x7A }, AmfString("quz").serializeValue(ctx));
	isEqual(v8 { 0x09, 0x71, 0x75, 0x75, 0x78 }, AmfString("quux").serializeValue(ctx));
}

TEST(StringSerialization, SerializeValueCache) {
	SerializationContext ctx;
	isEqual(v8 { 0x07, 0x62, 0x61, 0x72 }, AmfString("bar").serializeValue(ctx));
	isEqual(v8 { 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 }, AmfString("boofar").serializeValue(ctx));
	isEqual(v8 { 0x07, 0x71, 0x75, 0x78 }, AmfString("qux").serializeValue(ctx));
	isEqual(v8 { 0x00 }, AmfString("bar").serializeValue(ctx));
	isEqual(v8 { 0x04 }, AmfString("qux").serializeValue(ctx));
	isEqual(v8 { 0x06, 0x02 }, AmfString("boofar").serialize(ctx));
}

TEST(StringSerialization, EmtpyStringNotCached) {
	SerializationContext ctx;
	isEqual(v8 { 0x01 }, AmfString("").serializeValue(ctx));
	isEqual(v8 { 0x01 }, AmfString("").serializeValue(ctx));
	isEqual(v8 { 0x07, 0x62, 0x61, 0x72 }, AmfString("bar").serializeValue(ctx));
	isEqual(v8 { 0x00 }, AmfString("bar").serializeValue(ctx));
}

TEST(StringSerialization, Utf8VrCached) {
	SerializationContext ctx;
	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("bar", AmfString("qux"));

	isEqual(v8 {
		0x0a, 0x0b, 0x07, 0x66, 0x6f, 0x6f,
		0x07, 0x62, 0x61, 0x72,
		0x06, 0x07, 0x71, 0x75, 0x78,
		0x01
	}, obj.serialize(ctx));

	isEqual(v8 { 0x06, 0x00 }, AmfString("foo").serialize(ctx));
	isEqual(v8 { 0x06, 0x02 }, AmfString("bar").serialize(ctx));
	isEqual(v8 { 0x06, 0x04 }, AmfString("qux").serialize(ctx));
}

TEST(StringEquality, SimpleValues) {
	AmfString s1;
	AmfString s2("");
	EXPECT_EQ(s1, s1);
	EXPECT_EQ(s1, s2);

	AmfString s3("foo");
	AmfString s4("foo");
	EXPECT_EQ(s3, s4);
	EXPECT_NE(s1, s3);

	AmfString s5("foobar");
	EXPECT_NE(s3, s5);
}

TEST(StringEquality, StringConversion) {
	AmfString s1;
	EXPECT_EQ(std::string(s1), "");

	AmfString s2("foo");
	EXPECT_EQ(std::string(s2), "foo");

	AmfString s3("ħ");
	EXPECT_EQ(std::string(s3), "ħ");
}

TEST(StringEquality, MixedTypes) {
	AmfString s1("foo");
	AmfXml x1("foo");
	AmfXmlDocument x2("foo");
	AmfInteger i1(0x666f6f);
	EXPECT_NE(s1, x1);
	EXPECT_NE(s1, x2);
	EXPECT_NE(s1, i1);
}

static void deserializesTo(const char* value, const v8& data, int left = 0,
	DeserializationContext* ctx = nullptr) {
	deserialize(AmfString(value), data, left, ctx);
}

TEST(StringDeserialization, EmptyString) {
	deserializesTo("", { 0x06, 0x01 });
}

TEST(StringDeserialization, Ascii) {
	deserializesTo("bar", { 0x06, 0x07, 0x62, 0x61, 0x72 });
	deserializesTo("boofar", { 0x06, 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72 });
	deserializesTo("qux", { 0x06, 0x07, 0x71, 0x75, 0x78 });
	deserializesTo("quz", { 0x06, 0x07, 0x71, 0x75, 0x7A });
	deserializesTo("quux", { 0x06, 0x09, 0x71, 0x75, 0x75, 0x78 });
	deserializesTo("\"'\\", { 0x06, 0x07, 0x22, 0x27, 0x5C });
}

TEST(StringDeserialization, Umlaute) {
	deserializesTo("ümläütë", { 0x06, 0x17, 0xC3, 0xBC, 0x6D, 0x6C, 0xC3, 0xA4, 0xC3, 0xBC, 0x74, 0xC3, 0xAB });
}

TEST(StringDeserialization, Unicode) {
	deserializesTo("ħĸð@þæĸſ“ð", {
		0x06,
		0x29, 0xC4, 0xA7, 0xC4, 0xB8, 0xC3, 0xB0, 0x40, 0xC3, 0xBE, 0xC3, 0xA6,
		0xC4, 0xB8, 0xC5, 0xBF, 0xE2, 0x80, 0x9C, 0xC3, 0xB0 });
	deserializesTo("”]²³¶ŧ↓øħ”“łµæðµ→³øħ", {
		0x06,
		0x59, 0xE2, 0x80, 0x9D, 0x5D, 0xC2, 0xB2, 0xC2, 0xB3, 0xC2, 0xB6, 0xC5,
		0xA7, 0xE2, 0x86, 0x93, 0xC3, 0xB8, 0xC4, 0xA7, 0xE2, 0x80, 0x9D, 0xE2,
		0x80, 0x9C, 0xC5, 0x82, 0xC2, 0xB5, 0xC3, 0xA6, 0xC3, 0xB0, 0xC2, 0xB5,
		0xE2, 0x86, 0x92, 0xC2, 0xB3, 0xC3, 0xB8, 0xC4, 0xA7 });
}

TEST(StringDeserialization, BytesLeft) {
	deserializesTo("bar", { 0x06, 0x07, 0x62, 0x61, 0x72, 0x72 }, 1);
	deserializesTo("boofar", { 0x06, 0x0D, 0x62, 0x6F, 0x6F, 0x66, 0x61, 0x72,
		0x66, 0x61, 0x72 }, 3);
	deserializesTo("qux", { 0x06, 0x07, 0x71, 0x75, 0x78 }, 0);
}

TEST(StringDeserialization, MultiByteLength) {
	deserializesTo(std::string(300, 'a').c_str(), {
		0x06,
		0x84, 0x59, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61, 0x61,
		0x61, 0x61, 0x61, 0x61, 0x61 });
}

TEST(StringDeserialization, EmptyIterator) {
	v8 empty { };
	auto begin = empty.cbegin();
	DeserializationContext ctx;
	ASSERT_THROW({
		AmfString::deserialize(begin, empty.cend(), ctx);
	}, std::invalid_argument);
}

TEST(StringDeserialization, StringContext) {
	DeserializationContext ctx;
	deserializesTo("bar", { 0x06, 0x07, 0x62, 0x61, 0x72 }, 0, &ctx);
	deserializesTo("bar", { 0x06, 0x00 }, 0, &ctx);
	deserializesTo("foobar", { 0x06, 0x0D, 0x66, 0x6F, 0x6F, 0x62, 0x61, 0x72 }, 0, &ctx);
	deserializesTo("bar", { 0x06, 0x00 }, 0, &ctx);
	deserializesTo("qux", { 0x06, 0x07, 0x71, 0x75, 0x78 }, 0, &ctx);
	deserializesTo("foobar", { 0x06, 0x02 }, 0, &ctx);
	deserializesTo("qux", { 0x06, 0x04 }, 0, &ctx);
}

TEST(StringDeserialization, EmtpyStringNotCached) {
	DeserializationContext ctx;
	deserializesTo("", { 0x06, 0x01 }, 0, &ctx);
	deserializesTo("bar", { 0x06, 0x07, 0x62, 0x61, 0x72 }, 0, &ctx);
	deserializesTo("bar", { 0x06, 0x00 }, 0, &ctx);
}

TEST(StringDeserialization, Utf8VrCached) {
	DeserializationContext ctx;
	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("bar", AmfString("qux"));
	deserialize(obj, {
		0x0a, 0x0b, 0x07, 0x66, 0x6f, 0x6f,
		0x07, 0x62, 0x61, 0x72,
		0x06, 0x07, 0x71, 0x75, 0x78,
		0x01
	}, 0, &ctx);

	deserializesTo("foo", { 0x06, 0x00 }, 0, &ctx);
	deserializesTo("bar", { 0x06, 0x02 }, 0, &ctx);
	deserializesTo("qux", { 0x06, 0x04 }, 0, &ctx);
}

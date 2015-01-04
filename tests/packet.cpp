#include "amftest.hpp"

#include "amf.hpp"
#include "amfpacket.hpp"
#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"

TEST(PacketTest, SingleMessage) {
	AmfPacket packet;
	packet.messages.emplace_back("com/foo.bar", "/1/", AmfString("hello"));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x01, // message count: 1
		0x00, 0x0b, // target-uri length: 11
		// target-uri: "com/foo.bar"
		0x63, 0x6f, 0x6d, 0x2f, 0x66, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x03, // response-uri length: 3
		0x2f, 0x31, 0x2f, // response-uri: "/1/"
		0x00, 0x00, 0x00, 0x08, // value length: 8
		0x11, // AMF3 object marker
		0x06, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f // value: AmfString "hello"
	}, packet);

	packet = AmfPacket();
	packet.messages.emplace_back("de/ventero/AmfCpp.test", "/1/onResult", AmfInteger(27));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x01, // message count: 1
		0x00, 0x16, // target-uri length: 22 (0x16)
		// target-uri "de/ventero/AmfCpp.test"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f, 0x41,
		0x6d, 0x66, 0x43, 0x70, 0x70, 0x2e, 0x74, 0x65, 0x73, 0x74,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		0x00, 0x00, 0x00, 0x03, // value length: 3
		0x11, // AMF3 object marker
		0x04, 0x1b // AmfInteger 27
	}, packet);
}

TEST(PacketTest, MultipleMessages) {
	AmfPacket packet;
	packet.messages.emplace_back("com/foo.bar", "/1/", AmfString("hello"));
	packet.messages.emplace_back("de/ventero/AmfCpp.test", "/1/onResult", AmfInteger(27));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x02, // message count: 1
		// 1. message
		0x00, 0x0b, // target-uri length: 11
		// target-uri: "com/foo.bar"
		0x63, 0x6f, 0x6d, 0x2f, 0x66, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x03, // response-uri length: 3
		0x2f, 0x31, 0x2f, // response-uri: "/1/"
		0x00, 0x00, 0x00, 0x08, // value length: 8
		0x11, // AMF3 object marker
		0x06, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // value: AmfString "hello"
		// 2. message
		0x00, 0x16, // target-uri length: 22 (0x16)
		// target-uri "de/ventero/AmfCpp.test"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f, 0x41,
		0x6d, 0x66, 0x43, 0x70, 0x70, 0x2e, 0x74, 0x65, 0x73, 0x74,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		0x00, 0x00, 0x00, 0x03, // value length: 3
		0x11, // AMF3 object marker
		0x04, 0x1b // AmfInteger 27
	}, packet);
}

TEST(PacketTest, MultiByteMessageCount) {
	AmfPacket packet;
	PacketMessage message("a.b", "/1", AmfBool(false));

	for (int i = 0; i < 258; ++i)
		packet.messages.push_back(message);

	v8 serialized = packet.serialize();
	auto it = serialized.begin();

	size_t offset = 6;
	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x01, 0x02 // message count: 258
	}, v8(it, it + offset));
	it += offset;

	v8 expectedMessage = {
		0x00, 0x03, // target-uri length: 3
		0x61, 0x2e, 0x62, // target-uri: "a.b"
		0x00, 0x02, // response-uri length: 2
		0x2f, 0x31, // response-uri: "/1"
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF3 object marker
		0x02 // AmfBool false
	};

	size_t size = expectedMessage.size();
	for (int i = 0; i < 258; ++i) {
		isEqual(expectedMessage, v8(it, it + size));
		it += size;
	}

	ASSERT_EQ(it, serialized.end());
	ASSERT_EQ(3876, serialized.size());
}

TEST(PacketTest, SingleHeader) {
	AmfPacket packet;
	packet.headers.emplace_back("foo", false, AmfBool(false));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x03, // header name length: 3
		0x66, 0x6f, 0x6f, // header name: "foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF 3 object marker
		0x02, // AmfBool false
		0x00, 0x00 // message count: 0
	}, packet);

	packet.headers.clear();
	packet.headers.emplace_back("barqux", true, AmfDouble(-3.14159));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x06, // header name length: 6
		0x62, 0x61, 0x72, 0x71, 0x75, 0x78, // header name: "barqux"
		0x01, // must understand: true
		0x00, 0x00, 0x00, 0x0a, // value length: 2
		0x11, // AMF 3 object marker
		0x05, 0xc0, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e, // AmfDouble -3.14159
		0x00, 0x00 // message count: 0
	}, packet);
}

TEST(PacketTest, MultipleHeader) {
	AmfPacket packet;
	packet.headers.emplace_back("foo", false, AmfBool(false));
	packet.headers.emplace_back("barqux", true, AmfDouble(-3.14159));

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x02, // header count: 1
		0x00, 0x03, // header name length: 3
		0x66, 0x6f, 0x6f, // header name: "foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF 3 object marker
		0x02, // AmfBool false
		0x00, 0x06, // header name length: 6
		0x62, 0x61, 0x72, 0x71, 0x75, 0x78, // header name: "barqux"
		0x01, // must understand: true
		0x00, 0x00, 0x00, 0x0a, // value length: 2
		0x11, // AMF 3 object marker
		0x05, 0xc0, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e, // AmfDouble -3.14159
		0x00, 0x00 // message count: 0
	}, packet);
}

TEST(PacketTest, MultiByteHeaderCount) {
	AmfPacket packet;
	PacketHeader header("a", false, AmfBool(false));

	for (int i = 0; i < 258; ++i)
		packet.headers.push_back(header);

	v8 serialized = packet.serialize();
	auto it = serialized.begin();

	size_t offset = 4;
	isEqual({
		0x00, 0x03, // version: AMF 3
		0x01, 0x02, // header count: 258
	}, v8(it, it + offset));
	it += offset;

	v8 expectedHeader = {
		0x00, 0x01, // name length: 1
		0x61, // name: "a"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF3 object marker
		0x02 // AmfBool false
	};

	size_t size = expectedHeader.size();
	for (int i = 0; i < 258; ++i) {
		isEqual(expectedHeader, v8(it, it + size));
		it += size;
	}

	isEqual({
		0x00, 0x00 // message count: 0
	}, v8(it, it + 2));

	ASSERT_EQ(it + 2, serialized.end());
	ASSERT_EQ(2586, serialized.size());
}

TEST(PacketTest, HeaderAndMessage) {
	AmfPacket packet;
	packet.headers.emplace_back("Foo", false, AmfString("bar"));
	packet.messages.emplace_back("de/ventero/Foo.bar", "/1/onResult", AmfArray());

	isEqual({
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x03, // name length: 3
		0x46, 0x6f, 0x6f, // name: "Foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x06, // value length: 6
		0x11, // AMF3 object marker
		0x06, 0x07, 0x62, 0x61, 0x72, // AmfString "bar"
		0x00, 0x01, // message count: 1
		0x00, 0x12, // target-uri length: 18 (0x12)
		// target-uri: "de/Ventero/Foo.bar"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f,
		0x46, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		0x00, 0x00, 0x00, 0x04, // value length: 4
		0x11, // AMF3 object marker
		0x09, 0x01, 0x01 // empty AmfArray
	}, packet);
}

TEST(PacketTest, TooManyHeaders) {
	AmfPacket p;
	p.headers = std::vector<PacketHeader>(65536, PacketHeader("Foo", false, AmfNull()));
	ASSERT_THROW(p.serialize(), std::length_error);
}

TEST(PacketTest, TooManyMessages) {
	AmfPacket p;
	p.messages = std::vector<PacketMessage>(65536, PacketMessage("Foo", "", AmfNull()));
	ASSERT_THROW(p.serialize(), std::length_error);
}

TEST(PacketEquality, Header) {
	PacketHeader h1("foo", false, AmfNull());
	PacketHeader h2("foo", false, AmfUndefined());
	EXPECT_NE(h1, h2);

	PacketHeader h3("foo", true, AmfNull());
	EXPECT_NE(h1, h3);

	PacketHeader h4("bar", false, AmfNull());
	EXPECT_NE(h1, h4);

	PacketHeader h5("foo", false, AmfNull());
	EXPECT_EQ(h1, h5);
}

TEST(PacketEquality, Message) {
	PacketMessage h1("foo", "qux", AmfNull());
	PacketMessage h2("foo", "qux", AmfUndefined());
	EXPECT_NE(h1, h2);

	PacketMessage h3("foo", "quux", AmfNull());
	EXPECT_NE(h1, h3);

	PacketMessage h4("bar", "qux", AmfNull());
	EXPECT_NE(h1, h4);

	PacketMessage h5("foo", "qux", AmfNull());
	EXPECT_EQ(h1, h5);
}

TEST(PacketEquality, Packet) {
	AmfPacket p1, p2;
	EXPECT_EQ(p1, p2);

	p1.headers.emplace_back("foo", false, AmfInteger(0));
	EXPECT_NE(p1, p2);
	p2.headers.push_back(p1.headers.at(0));
	EXPECT_EQ(p1, p2);

	p2.messages.emplace_back("qux", "zzz", AmfString("foo"));
	EXPECT_NE(p1, p2);
	p1.messages.push_back(p2.messages.at(0));
	EXPECT_EQ(p1, p2);
}

TEST(PacketEquality, MixedTypes) {
	AmfPacket p;
	PacketHeader h("foo", false, AmfNull());
	PacketMessage m("foo", "", AmfNull());
	AmfNull n;

	EXPECT_NE(p, h);
	EXPECT_NE(p, m);
	EXPECT_NE(p, n);
}

template<typename T>
static void deserializesTo(T value, const v8& data, int left = 0,
	DeserializationContext* ctx = nullptr) {
	deserialize(value, data, left, ctx);
}

TEST(PacketDeserialization, SingleMessage) {
	AmfPacket packet;
	packet.messages.emplace_back("com/foo.bar", "/1/", AmfString("hello"));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x01, // message count: 1
		0x00, 0x0b, // target-uri length: 11
		// target-uri: "com/foo.bar"
		0x63, 0x6f, 0x6d, 0x2f, 0x66, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x03, // response-uri length: 3
		0x2f, 0x31, 0x2f, // response-uri: "/1/"
		0x00, 0x00, 0x00, 0x08, // value length: 8
		0x11, // AMF3 object marker
		0x06, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f // value: AmfString "hello"
	});

	packet = AmfPacket();
	packet.messages.emplace_back("de/ventero/AmfCpp.test", "/1/onResult", AmfInteger(27));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x01, // message count: 1
		0x00, 0x16, // target-uri length: 22 (0x16)
		// target-uri "de/ventero/AmfCpp.test"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f, 0x41,
		0x6d, 0x66, 0x43, 0x70, 0x70, 0x2e, 0x74, 0x65, 0x73, 0x74,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		0x00, 0x00, 0x00, 0x03, // value length: 3
		0x11, // AMF3 object marker
		0x04, 0x1b // AmfInteger 27
	});
}

TEST(PacketDeserialization, MultipleMessages) {
	AmfPacket packet;
	packet.messages.emplace_back("com/foo.bar", "/1/", AmfString("hello"));
	packet.messages.emplace_back("de/ventero/AmfCpp.test", "/1/onResult", AmfInteger(27));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x00, 0x02, // message count: 1
		// 1. message
		0x00, 0x0b, // target-uri length: 11
		// target-uri: "com/foo.bar"
		0x63, 0x6f, 0x6d, 0x2f, 0x66, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x03, // response-uri length: 3
		0x2f, 0x31, 0x2f, // response-uri: "/1/"
		0x00, 0x00, 0x00, 0x08, // value length: 8
		0x11, // AMF3 object marker
		0x06, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f, // value: AmfString "hello"
		// 2. message
		0x00, 0x16, // target-uri length: 22 (0x16)
		// target-uri "de/ventero/AmfCpp.test"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f, 0x41,
		0x6d, 0x66, 0x43, 0x70, 0x70, 0x2e, 0x74, 0x65, 0x73, 0x74,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		0x00, 0x00, 0x00, 0x03, // value length: 3
		0x11, // AMF3 object marker
		0x04, 0x1b // AmfInteger 27
	});
}

TEST(PacketDeserialization, MultiByteMessageCount) {
	AmfPacket packet;
	PacketMessage message("a.b", "/1", AmfBool(false));

	for (int i = 0; i < 258; ++i)
		packet.messages.push_back(message);

	v8 data {
		0x00, 0x03, // version: AMF 3
		0x00, 0x00, // header count: 0
		0x01, 0x02 // message count: 258
	};

	v8 expectedMessage = {
		0x00, 0x03, // target-uri length: 3
		0x61, 0x2e, 0x62, // target-uri: "a.b"
		0x00, 0x02, // response-uri length: 2
		0x2f, 0x31, // response-uri: "/1"
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF3 object marker
		0x02 // AmfBool false
	};

	data.reserve(data.size() + 258 * expectedMessage.size());
	for (int i = 0; i < 258; ++i)
		data.insert(data.end(), expectedMessage.begin(), expectedMessage.end());

	deserializesTo(packet, data);
}

TEST(PacketDeserialization, SingleHeader) {
	AmfPacket packet;
	packet.headers.emplace_back("foo", false, AmfBool(false));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x03, // header name length: 3
		0x66, 0x6f, 0x6f, // header name: "foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF 3 object marker
		0x02, // AmfBool false
		0x00, 0x00 // message count: 0
	});

	packet.headers.clear();
	packet.headers.emplace_back("barqux", true, AmfDouble(-3.14159));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x06, // header name length: 6
		0x62, 0x61, 0x72, 0x71, 0x75, 0x78, // header name: "barqux"
		0x01, // must understand: true
		0x00, 0x00, 0x00, 0x0a, // value length: 2
		0x11, // AMF 3 object marker
		0x05, 0xc0, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e, // AmfDouble -3.14159
		0x00, 0x00 // message count: 0
	});
}

TEST(PacketDeserialization, MultipleHeader) {
	AmfPacket packet;
	packet.headers.emplace_back("foo", false, AmfBool(false));
	packet.headers.emplace_back("barqux", true, AmfDouble(-3.14159));

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x02, // header count: 1
		0x00, 0x03, // header name length: 3
		0x66, 0x6f, 0x6f, // header name: "foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF 3 object marker
		0x02, // AmfBool false
		0x00, 0x06, // header name length: 6
		0x62, 0x61, 0x72, 0x71, 0x75, 0x78, // header name: "barqux"
		0x01, // must understand: true
		0x00, 0x00, 0x00, 0x0a, // value length: 2
		0x11, // AMF 3 object marker
		0x05, 0xc0, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e, // AmfDouble -3.14159
		0x00, 0x00 // message count: 0
	});
}

TEST(PacketDeserialization, MultiByteHeaderCount) {
	AmfPacket packet;
	PacketHeader header("a", false, AmfBool(false));

	for (int i = 0; i < 258; ++i)
		packet.headers.push_back(header);

	v8 data = {
		0x00, 0x03, // version: AMF 3
		0x01, 0x02, // header count: 258
	};

	v8 expectedHeader = {
		0x00, 0x01, // name length: 1
		0x61, // name: "a"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x02, // value length: 2
		0x11, // AMF3 object marker
		0x02 // AmfBool false
	};

	data.reserve(data.size() + 258 * expectedHeader.size() + 2);
	for (int i = 0; i < 258; ++i)
		data.insert(data.end(), expectedHeader.begin(), expectedHeader.end());

	// Message count
	data.push_back(0x00);
	data.push_back(0x00);

	deserializesTo(packet, data);
}

TEST(PacketDeserialization, MessageAndHeader) {
	AmfPacket packet;
	packet.headers.emplace_back("Foo", false, AmfString("bar"));
	packet.messages.emplace_back("de/ventero/Foo.bar", "/1/onResult", AmfArray());

	deserializesTo(packet, {
		0x00, 0x03, // version: AMF 3
		0x00, 0x01, // header count: 1
		0x00, 0x03, // name length: 3
		0x46, 0x6f, 0x6f, // name: "Foo"
		0x00, // must understand: false
		0x00, 0x00, 0x00, 0x06, // value length: 6
		0x11, // AMF3 object marker
		0x06, 0x07, 0x62, 0x61, 0x72, // AmfString "bar"
		0x00, 0x01, // message count: 1
		0x00, 0x12, // target-uri length: 18 (0x12)
		// target-uri: "de/Ventero/Foo.bar"
		0x64, 0x65, 0x2f, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2f,
		0x46, 0x6f, 0x6f, 0x2e, 0x62, 0x61, 0x72,
		0x00, 0x0b, // response-uri length: 11 (0x0b)
		// response-uri: "/1/onResult"
		0x2f, 0x31, 0x2f, 0x6f, 0x6e, 0x52, 0x65, 0x73, 0x75, 0x6c, 0x74,
		// 0x00, 0x00, 0x00, 0x04, // value length: 4
		0xff, 0xff, 0xff, 0xff,
		0x11, // AMF3 object marker
		0x09, 0x01, 0x01 // empty AmfArray
	});
}

TEST(PacketDeserialization, FlexMessage) {
	AmfObject value("flex.messaging.messages.RemotingMessage", true, false);
	value.addDynamicProperty("body", AmfArray(std::vector<AmfString> { "first_arg", "second_arg" }));
	value.addDynamicProperty("clientId", AmfNull());
	value.addDynamicProperty("destination", AmfNull());
	value.addDynamicProperty("headers", AmfObject("", true, false));
	value.addDynamicProperty("messageId", AmfString("9D108E33-B591-BE79-210D-F1A72D06B578"));
	value.addDynamicProperty("operation", AmfString("test"));
	value.addDynamicProperty("source", AmfString("TestController"));
	value.addDynamicProperty("timeToLive", AmfInteger(0));
	value.addDynamicProperty("timestamp", AmfInteger(0));

	AmfPacket packet;
	packet.messages.emplace_back("null", "/2", value);

	deserializesTo(packet, {
		0x00, 0x03, // AMF 3
		0x00, 0x00, // 0 headers
		0x00, 0x01, // 1 message
		0x00, 0x04, // target uri length 4
		0x6e, 0x75, 0x6c, 0x6c, // target uri "null"
		0x00, 0x02, // response uri length 2
		0x2f, 0x32, // response  "/2"
		0xff, 0xff, 0xff, 0xff, // unknown object length
		// 0x00, 0x00, 0x00, 0xdc, // object length: 220 (0xdc)
		0x11, // AMF 3 object marker
		0x0a, // value: AmfObject
		0x0b, // U29O-traits dynamic, 0 sealed traits
		0x4f, // class name length 37
		// class name "flex.messaging.messages.RemotingMessage"
		0x66, 0x6c, 0x65, 0x78, 0x2e, 0x6d, 0x65, 0x73, 0x73, 0x61,
		0x67, 0x69, 0x6e, 0x67, 0x2e, 0x6d, 0x65, 0x73, 0x73, 0x61,
		0x67, 0x65, 0x73, 0x2e, 0x52, 0x65, 0x6d, 0x6f, 0x74, 0x69,
		0x6e, 0x67, 0x4d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65,
		// 1. dynamic member
			// key: AmfString "body"
			0x09, 0x62, 0x6f, 0x64, 0x79,
			// value: AmfArray, 2 dense elements, 0 assoc elements
			0x09, 0x05, 0x01,
			// AmfString "first_arg"
			0x06, 0x13, 0x66, 0x69, 0x72, 0x73, 0x74, 0x5f, 0x61, 0x72, 0x67,
			// AmfString "second_arg"
			0x06, 0x15, 0x73, 0x65, 0x63, 0x6f, 0x6e, 0x64, 0x5f, 0x61, 0x72, 0x67,
		// 2. dynamic member
			// key: AmfString "clientId"
			0x11, 0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x49, 0x64,
			// value: null
			0x01,
		// 3. dynamic member
			// key: AmfString "destination"
			0x17, 0x64, 0x65, 0x73, 0x74, 0x69, 0x6e, 0x61, 0x74, 0x69, 0x6f, 0x6e,
			// value: null
			0x01,
		// 4. dynamic member
			// key: AmfString "headers"
			0x0f, 0x68, 0x65, 0x61, 0x64, 0x65, 0x72, 0x73,
			// value: AmfObject, U29O-traits dynamic, 0 sealed traits
			0x0a, 0x0b,
			// anonymous object
			0x01,
			// end of object
			0x01,
		// 5. dynamic member
			// key: AmfString "messageId"
			0x13, 0x6d, 0x65, 0x73, 0x73, 0x61, 0x67, 0x65, 0x49, 0x64,
			// value: AmfString "9D108E33-B591-BE79-210D-F1A72D06B578"
			0x06, 0x49,
			0x39, 0x44, 0x31, 0x30, 0x38, 0x45, 0x33, 0x33, 0x2d, 0x42,
			0x35, 0x39, 0x31, 0x2d, 0x42, 0x45, 0x37, 0x39, 0x2d, 0x32,
			0x31, 0x30, 0x44, 0x2d, 0x46, 0x31, 0x41, 0x37, 0x32, 0x44,
			0x30, 0x36, 0x42, 0x35, 0x37, 0x38,
		// 6. dynamic member
			// key: AmfString "operation"
			0x13, 0x6f, 0x70, 0x65, 0x72, 0x61, 0x74, 0x69, 0x6f, 0x6e,
			// value: AmfString "test"
			0x06, 0x09, 0x74, 0x65, 0x73, 0x74,
		// 7. dynamic member
			// key: AmfString "source"
			0x0d, 0x73, 0x6f, 0x75, 0x72, 0x63, 0x65,
			// value: AmfString "TestController"
			0x06, 0x1d, 0x54, 0x65, 0x73, 0x74, 0x43, 0x6f, 0x6e, 0x74,
			0x72, 0x6f, 0x6c, 0x6c, 0x65, 0x72,
		// 8. dynamic member
			// key: AmfString "timeToLive"
			0x15, 0x74, 0x69, 0x6d, 0x65, 0x54, 0x6f, 0x4c, 0x69, 0x76, 0x65,
			// value: AmfInteger(0)
			0x04, 0x00,
		// 9. dynamic member
			// key: AmfString "timestamp"
			0x13, 0x74, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70,
			// value: AmfInteger(0)
			0x04, 0x00,
		// end of object
		0x01
	});
}

TEST(PacketDeserialization, InvalidVersion) {
	DeserializationContext ctx;

	v8 data {
		0x00, 0x04, // invalid version 4
		0x00, 0x00, // 0 headers
		0x00, 0x00 // 0 messages
	};
	auto it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::invalid_argument);

	data = {
		0xff, 0xff, // invalid version 0xffff
		0x00, 0x00, 0x00, 0x00 // 0 headers/messages
	};
	it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(PacketDeserialization, NotEnoughData) {
	DeserializationContext ctx;

	v8 data { };
	auto it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00 // partial version
	};
	it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		// no header count
	};
	it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		0x00 // partial header count
	};
	it = data.cbegin();
	ASSERT_THROW(AmfPacket::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		0x00, 0x00, // header count 0
		// no message count
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		0x00, 0x00, // header count 0
		0x00, // partial message count
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		0x00, 0x00, // header count 0
		0x00, 0x01, // message count 1
		// no message
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // version
		0x00, 0x01, // header count 1
		// no header
		0x00, 0x00, // message count 0
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);
}

TEST(HeaderDeserialization, SimpleHeader) {
	deserializesTo(PacketHeader("foo", false, AmfNull()), {
		0x00, 0x03, // length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});

	deserializesTo(PacketHeader("foo", true, AmfNull()), {
		0x00, 0x03, // length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x01, // must understand: true,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(HeaderDeserialization, EmptyName) {
	deserializesTo(PacketHeader("", false, AmfNull()), {
		0x00, 0x00, // name length: 0
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(HeaderDeserialization, UnknownValueLength) {
	deserializesTo(PacketHeader("foo", false, AmfNull()), {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0xff, 0xff, 0xff, 0xff, // unknown length
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(HeaderDeserialization, NotEnoughData) {
	DeserializationContext ctx;

	v8 data {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x03, // length: 3
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x04, // name length: 4
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x03, // length: 3
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, // "foo"
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00 // partial name length
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x00 // partial value length
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x03, // length: 3
	};
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = { };
	it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::out_of_range);
}

TEST(HeaderDeserialization, InvalidTypeMarker) {
	DeserializationContext ctx;
	v8 data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x10, // not AVMPLUS_OBJECT
		0x01 // AmfNull
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(HeaderDeserialization, InvalidValueMarker) {
	DeserializationContext ctx;
	v8 data = {
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, // must understand: false,
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0xff // invalid amf marker
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketHeader::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(MessageDeserialization, SimpleMessage) {
	deserializesTo(PacketMessage("foo", "bar", AmfNull()), {
		0x00, 0x03, // length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x03, // length 3
		0x62, 0x61, 0x72, // "bar"
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});

	deserializesTo(PacketMessage("foo", "foobar", AmfNull()), {
		0x00, 0x03, // length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x06, // length 6
		0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72, // "bar"
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(MessageDeserialization, EmptyTarget) {
	deserializesTo(PacketMessage("", "foo", AmfNull()), {
		0x00, 0x00, // target length: 0
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(MessageDeserialization, EmptyResponse) {
	deserializesTo(PacketMessage("foo", "", AmfNull()), {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x00, // response length: 0
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(MessageDeserialization, UnknownValueLength) {
	deserializesTo(PacketMessage("foo", "foo", AmfNull()), {
		0x00, 0x03, // length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x03, // name length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0xff, 0xff, 0xff, 0xff, // unknown length
		0x11, // AVMPLUS_OBJECT
		0x01 // AmfNull
	});
}

TEST(MessageDeserialization, NotEnoughData) {
	DeserializationContext ctx;

	v8 data {
		0x00 // partial target length
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, // target too short
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, // partial response length
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		// empty response
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x00 // partial response
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // response "foo"
		0x00, 0x00 // partial value length
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // response "foo"
		0x00, 0x00, 0x00, 0x01 // value length 1
		// no AVM plus marker and value
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // response "foo"
		0x00, 0x00, 0x00, 0x01, // value length 1
		0x11, // AVM plus marker
		// no value, but matching byte count
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // response "foo"
		0x00, 0x00, 0x00, 0x02, // value length 2
		0x11, // AVM plus marker
		// no value
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);

	data = {
		0x00, 0x03, // target length 3
		0x66, 0x6f, 0x6f, // target "foo"
		0x00, 0x03, // response length 3
		0x66, 0x6f, 0x6f, // response "foo"
		0x00, 0x00, 0x00, 0x03, // value length 3
		0x11, // AVM plus marker
		0x01, // AmfNull()
		// value too short
	};
	it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::out_of_range);
}

TEST(MessageDeserialization, InvalidTypeMarker) {
	DeserializationContext ctx;
	v8 data = {
		0x00, 0x03, // length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x03, // length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x10, // not AVMPLUS_OBJECT
		0x01 // AmfNull
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(MessageDeserialization, InvalidValueMarker) {
	DeserializationContext ctx;
	v8 data = {
		0x00, 0x03, // length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x03, // length: 3
		0x66, 0x6f, 0x6f, // "foo"
		0x00, 0x00, 0x00, 0x02, // length: 2
		0x11, // AVMPLUS_OBJECT
		0xff // invalid amf marker
	};
	auto it = data.cbegin();
	ASSERT_THROW(PacketMessage::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

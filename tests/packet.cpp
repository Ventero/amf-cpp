#include "amftest.hpp"

#include "amf.hpp"
#include "amfpacket.hpp"
#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfnull.hpp"
#include "types/amfstring.hpp"

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

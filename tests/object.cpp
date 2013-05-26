#include "gtest/gtest.h"

#include "amf.hpp"
#include "types/amfobject.hpp"

void isEqual(const std::vector<u8>& expected, const AmfObject& value) {
	v8 serialized = value.serialize();
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();
}

TEST(ObjectSerializationTest, EmptyDynamicAnonymousObject) {
	AmfObjectTraits traits("", true, false);
	AmfObject obj(traits);

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x01, // class-name "" (anonymous object)
		0x01  // end of object (UTF-8-empty)
	}, obj);
}

TEST(ObjectSerializationTest, DynamicAnonymousObject) {
	AmfObjectTraits traits("", true, false);
	AmfObject obj(traits);

	AmfString value("val");
	obj.dynamicProperties["prop"] = &value;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x01, // class-name "" (anonymous object)
		// dynamic-member
		0x09, 0x70, 0x72, 0x6f, 0x70, // UTF-8-vr "prop"
		0x06, 0x07, 0x76, 0x61, 0x6c, // AmfString "val"
		0x01 // end of object (UTF-8-empty)
	}, obj);
}

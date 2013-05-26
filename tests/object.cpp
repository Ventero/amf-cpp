#include "gtest/gtest.h"

#include "amf.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"

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

TEST(ObjectSerializationTest, SealedAnonymousObject) {
	{
		AmfObjectTraits traits("", false, false);
		traits.attributes.push_back("sealedProp");
		AmfObject obj(traits);

		AmfString value("value");
		obj.sealedProperties["sealedProp"] = &value;

		isEqual(v8 {
			0x0a, // AMF_OBJECT
			0x13, // 0b10011, U29O-traits, not dynamic, 1 sealed property
			0x01, // class-name "" (anonymous object)
			// sealed property names
			// UTF-8-vr "sealedProp"
			0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
			// sealed property values
			// AmfString "value"
			0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65
			// no dynamic members, so no empty string
		}, obj);
	}

	{
		AmfObjectTraits traits("", false, false);
		traits.attributes.push_back("sealedProp");
		traits.attributes.push_back("otherSealedProp");
		AmfObject obj(traits);

		AmfString value("value");
		obj.sealedProperties["sealedProp"] = &value;
		AmfString otherValue("otherValue");
		obj.sealedProperties["otherSealedProp"] = &otherValue;

		isEqual(v8 {
			0x0a, // AMF_OBJECT
			0x23, // 0b100011, U29O-traits, not dynamic, 2 sealed properties
			0x01, // class-name "" (anonymous object)
			// sealed property names
			// UTF-8-vr "sealedProp"
			0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
			// UTF-8-vr "otherSealedProp"
			0x1f, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x53, 0x65, 0x61, 0x6c, 0x65, 0x64,
			0x50, 0x72, 0x6f, 0x70,
			// sealed propety values
			// AmfString "value"
			0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65,
			// AmfString "otherValue"
			0x06, 0x15, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x56, 0x61, 0x6c, 0x75, 0x65
			// no dynamic members
		}, obj);
	}
}

TEST(ObjectSerializationTest, DynamicSealedAnonymousObject) {
	AmfObjectTraits traits("", true, false);
	traits.attributes.push_back("sealedProp");

	AmfObject obj(traits);

	AmfString sealedValue("value");
	obj.sealedProperties["sealedProp"] = &sealedValue;
	AmfString dynamicValue("dynamicValue");
	obj.dynamicProperties["dynamicProp"] = &dynamicValue;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x1b, // 0b11011, U29O-traits, dynamic, 1 sealed property
		0x01, // class-name "" (anonymous object)
		// sealed property names
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property values
		// AmfString "value"
		0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65,
		// dynamic members
		// UTF-8-vr "dynamicProp"
		0x17, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x50, 0x72, 0x6f, 0x70,
		// AmfString "dynamicValue"
		0x06, 0x19, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x56, 0x61, 0x6c, 0x75, 0x65,
		// end of dynamic members
		0x01
	}, obj);
}

TEST(ObjectSerializationTest, SealedNamedObject) {
	AmfObjectTraits traits("de.ventero.AmfTest", false, false);
	traits.attributes.push_back("sealedProp");
	AmfObject obj(traits);

	AmfString sealedValue("value");
	obj.sealedProperties["sealedProp"] = &sealedValue;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x13, // 0b10011, U29O-traits, not dynamic, 1 sealed property
		// class-name UTF-8-vr "de.ventero.AmfTest"
		0x25, 0x64, 0x65, 0x2e, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2e,
		0x41, 0x6d, 0x66, 0x54, 0x65, 0x73, 0x74,
		// sealed property names
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property values
		// AmfString "value"
		0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65
		// no dynamic members
	}, obj);
}

TEST(ObjectSerializationTest, DynamicSealedNamedObject) {
	AmfObjectTraits traits("de.ventero.AmfTest", true, false);
	traits.attributes.push_back("sealedProp");
	AmfObject obj(traits);

	AmfDouble sealedValue(3.14159);
	obj.sealedProperties["sealedProp"] = &sealedValue;

	AmfInteger dynamicValue(17);
	obj.dynamicProperties["dynamicProp"] = &dynamicValue;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x1b, // 0b11011, U29O-traits, dynamic, 1 sealed property
		// class-name UTF-8-vr "de.ventero.AmfTest"
		0x25, 0x64, 0x65, 0x2e, 0x76, 0x65, 0x6e, 0x74, 0x65, 0x72, 0x6f, 0x2e,
		0x41, 0x6d, 0x66, 0x54, 0x65, 0x73, 0x74,
		// sealed property names
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property values
		// AmfDouble 3.14159
		0x05, 0x40, 0x09, 0x21, 0xf9, 0xf0, 0x1b, 0x86, 0x6e,
		// dynamic members
		// UTF-8-vr dynamicProp
		0x17, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x50, 0x72, 0x6f, 0x70,
		// AmfInteger 17
		0x04, 0x11,
		// end of dynamic members
		0x01
	}, obj);
}

TEST(ObjectSerializationTest, EmptySealedAnonymousObject) {
	AmfObjectTraits traits("", false, false);
	AmfObject obj(traits);

	AmfInteger sealedValue(10);
	obj.sealedProperties["sealedProp"] = &sealedValue;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // 0b0011, U29O-traits, not dynamic, 0 sealed properties
		0x01 // class-name "" (anonymous object)
	}, obj);
}

TEST(ObjectSerializationTest, SerializeOnlyPropsInTraits) {
	AmfObjectTraits traits("", false, false);
	traits.attributes.push_back("sealedProp");
	AmfObject obj(traits);

	AmfInteger sealedValue(0x05ffeffe);
	obj.sealedProperties["sealedProp"] = &sealedValue;

	// this should not be serialized as it's not part of the trait attributes
	AmfString unusedValue("unused");
	obj.sealedProperties["unusedProp"] = &unusedValue;

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x13, // 0b10011, U29O-traits, not dynamic, 1 sealed property
		0x01, // class-name "" (anonymous object)
		// sealed property names
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property values
		// AmfInteger 0x05ffeffe
		0x04, 0x97, 0xff, 0xef, 0xfe
		// no dynamic members
	}, obj);
}

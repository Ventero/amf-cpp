#include "amftest.hpp"

#include "amf.hpp"
#include "deserializer.hpp"
#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"

TEST(ObjectSerialization, EmptyDynamicAnonymousObject) {
	AmfObject obj("", true, false);

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x01, // class-name "" (anonymous object)
		0x01  // end of object (UTF-8-empty)
	}, obj);
}

TEST(ObjectSerialization, DynamicAnonymousObject) {
	AmfObject obj("", true, false);

	obj.addDynamicProperty("prop", AmfString("val"));

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

TEST(ObjectSerialization, SealedAnonymousObject) {
	{
		SCOPED_TRACE("One property");
		AmfObject obj("", false, false);
		obj.addSealedProperty("sealedProp", AmfString("value"));

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
		SCOPED_TRACE("Two properties");
		AmfObject obj("", false, false);

		obj.addSealedProperty("sealedProp", AmfString("value"));
		obj.addSealedProperty("otherSealedProp", AmfString("otherValue"));

		isEqual(v8 {
			0x0a, // AMF_OBJECT
			0x23, // 0b100011, U29O-traits, not dynamic, 2 sealed properties
			0x01, // class-name "" (anonymous object)
			// sealed property names
			// UTF-8-vr "otherSealedProp"
			0x1f, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x53, 0x65, 0x61, 0x6c, 0x65, 0x64,
			0x50, 0x72, 0x6f, 0x70,
			// UTF-8-vr "sealedProp"
			0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
			// sealed propety values
			// AmfString "otherValue"
			0x06, 0x15, 0x6f, 0x74, 0x68, 0x65, 0x72, 0x56, 0x61, 0x6c, 0x75, 0x65,
			// AmfString "value"
			0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65,
			// no dynamic members
		}, obj);
	}
}

TEST(ObjectSerialization, DynamicSealedAnonymousObject) {
	AmfObject obj("", true, false);

	obj.addSealedProperty("sealedProp", AmfString("value"));
	obj.addDynamicProperty("dynamicProp", AmfString("dynamicValue"));

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

TEST(ObjectSerialization, SealedNamedObject) {
	AmfObject obj("de.ventero.AmfTest", false, false);

	obj.addSealedProperty("sealedProp", AmfString("value"));

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

TEST(ObjectSerialization, DynamicSealedNamedObject) {
	AmfObject obj("de.ventero.AmfTest", true, false);

	obj.addSealedProperty("sealedProp", AmfDouble(3.14159));
	obj.addDynamicProperty("dynamicProp", AmfInteger(17));

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

TEST(ObjectSerialization, EmptySealedAnonymousObject) {
	AmfObject obj("", false, false);

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // 0b0011, U29O-traits, not dynamic, 0 sealed properties
		0x01 // class-name "" (anonymous object)
	}, obj);
}

TEST(ObjectSerialization, SerializeOnlyPropsInTraits) {
	AmfObject obj("", false, false);

	obj.addSealedProperty("sealedProp", AmfInteger(0x05ffeffe));

	// this should not be serialized as it's not part of the trait attributes
	obj.sealedProperties["unusedProp"] = AmfItemPtr(new AmfString("unused"));

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

TEST(ObjectSerialization, NoDynamicPropOnSealedObject) {
	AmfObject obj;
	obj.addDynamicProperty("foo", AmfInteger(17));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // U29O-traits, not dynamic, 0 sealed properties
		0x01 // class name ""
		// no dynamic property
	}, obj);
}

TEST(ObjectSerialization, OverwriteProperties) {
	AmfObject o("", true, false);
	o.addSealedProperty("s", AmfInteger(0));
	o.addDynamicProperty("d", AmfBool(false));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x1b, // U29O-traits, dynamic, 1 sealed property
		0x01, // class name ""
		0x03, 0x73, // sealed property name UTF-8-vr "s"
		0x04, 0x00, // sealed property value AmfInteger(0)
		0x03, 0x64, // dynamic property name UTF-8-vr "d"
		0x02, // dynamic property value AmfFalse
		0x01 // end of dynamic properties
	}, o);

	o.addSealedProperty("s", AmfInteger(1));
	o.addDynamicProperty("d", AmfBool(true));
	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x1b, // U29O-traits, dynamic, 1 sealed property
		0x01, // class name ""
		0x03, 0x73, // sealed property name UTF-8-vr "s"
		0x04, 0x01, // sealed property value AmfInteger(1)
		0x03, 0x64, // dynamic property name UTF-8-vr "d"
		0x03, // dynamic property value AmfTrue
		0x01 // end of dynamic properties
	}, o);
}

TEST(ObjectSerialization, NonTraitCtor) {
	AmfObject obj;
	obj.addSealedProperty("sealedProp", AmfInteger(0x7b));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x13, // U29O-traits, not dynamic, 1 sealed prop
		0x01, // class-name ""
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property value
		// AmfInteger 0x7b
		0x04, 0x7b
		// no dynamic members
	}, obj);

	obj = AmfObject("foo", true, false);
	obj.addSealedProperty("sealedProp", AmfInteger(0x7b));
	obj.addDynamicProperty("dynamicProp", AmfString("dyn"));
	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x1b, // U29O-traits, dynamic, 1 sealed prop
		// class-name UTF-8-vr "foo"
		0x07, 0x66, 0x6f, 0x6f,
		// UTF-8-vr "sealedProp"
		0x15, 0x73, 0x65, 0x61, 0x6c, 0x65, 0x64, 0x50, 0x72, 0x6f, 0x70,
		// sealed property value
		// AmfInteger 0x7b
		0x04, 0x7b,
		// 1 dynamic member
		// UTF-8-vr "dynamicProp"
		0x17, 0x64, 0x79, 0x6e, 0x61, 0x6d, 0x69, 0x63, 0x50, 0x72, 0x6f, 0x70,
		// AmfString "dyn"
		0x06, 0x07, 0x64, 0x79, 0x6e,
		// end of dynamic members
		0x01
	}, obj);
}

TEST(ObjectSerialization, OnlySerializeDynamicPropsOnDynamicObjects) {
	// non-dynamic object
	AmfObject obj("", false, false);
	obj.addDynamicProperty("dynamicProp", AmfString("val"));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // 0b0011, U29O-traits, not dynamic, 0 sealed properties
		0x01 // class-name "" (anonymous object)
	}, obj);
}

TEST(ObjectSerialization, Externalizable) {
	auto externalizer = [] (const AmfObject* o) -> v8 {
		return v8 { u8(o->sealedProperties.size() * 3) };
	};

	AmfObject obj("foo", false, true);
	obj.externalizer = externalizer;

	isEqual(v8 {
		// AMF_OBJECT
		0x0a,
		// U29O-traits-ext
		0x07,
		// class name "foo"
		0x07, 0x66, 0x6f, 0x6f,
		// 3 * 0 members
		0x00
	}, obj);

	AmfObject obj2("foo", true, true);
	obj2.externalizer = externalizer;
	isEqual(v8 {
		// identical to the one above, as dynamic = true shouldn't make a difference
		0x0a,
		0x07,
		0x07, 0x66, 0x6f, 0x6f,
		0x00
	}, obj2);

	obj.addSealedProperty("foo", AmfString("bar"));
	isEqual(v8 {
		// AMF_OBJECT
		0x0a,
		// U29O-traits-ext
		0x07,
		// class name "foo"
		0x07, 0x66, 0x6f, 0x6f,
		// 3 * 1 members
		0x03
	}, obj);

	obj.addDynamicProperty("dyn", AmfBool(true));
	isEqual(v8 {
		// again, identical to the one above
		0x0a,
		0x07,
		0x07, 0x66, 0x6f, 0x6f,
		0x03
	}, obj);

	obj.addSealedProperty("a", AmfUndefined());
	obj.addSealedProperty("b", AmfNull());
	isEqual(v8 {
		0x0a,
		0x07,
		0x07, 0x66, 0x6f, 0x6f,
		// 3 properties -> 9
		0x09
	}, obj);

	auto propNameSerializer = [] (const AmfObject* o) -> v8 {
		v8 buf;
		SerializationContext ctx;
		for (const auto& it : o->dynamicProperties) {
			v8 s = AmfString(it.first).serialize(ctx);
			buf.insert(buf.end(), s.begin(), s.end());
		}
		return buf;
	};
	AmfObject obj3("x", false, true);
	obj3.externalizer = propNameSerializer;
	obj3.addDynamicProperty("foo", AmfInteger(1));
	isEqual(v8 {
		// AMF_OBJECT
		0x0a,
		// U29O-traits-ext
		0x07,
		// class name "x"
		0x03, 0x78,
		// AmfString "foo"
		0x06, 0x07, 0x66, 0x6f, 0x6f
	}, obj3);

	obj3.addDynamicProperty("foo", AmfString("overwritten"));
	isEqual(v8 {
		// same as above
		0x0a,
		0x07,
		0x03, 0x78,
		0x06, 0x07, 0x66, 0x6f, 0x6f
	}, obj3);
}

TEST(ObjectSerialization, ExternalizableThrowsWithoutExternalizer) {
	SerializationContext ctx;
	AmfObject obj("", true, true);
	ASSERT_THROW(obj.serialize(ctx), std::bad_function_call);
}

TEST(ObjectSerialization, PropertyCache) {
	AmfObject obj("", true, false);
	AmfObject inner;
	obj.addDynamicProperty("foo", inner);
	obj.addSealedProperty("bar", inner);

	SerializationContext ctx;
	isEqual({
		0x0a, 0x1b, 0x01,
		0x07, 0x62, 0x61, 0x72,
		0x0a, 0x03, 0x01,
		0x07, 0x66, 0x6f, 0x6f,
		0x0a, 0x02,
		0x01
	}, obj.serialize(ctx));
}

TEST(ObjectSerialization, TraitRefs) {
	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("foo", AmfNull());

	SerializationContext ctx;
	isEqual({
		0x0a, 0x0b,
		0x07, 0x66, 0x6f, 0x6f,
		0x00,
		0x01,
		0x01
	}, obj.serialize(ctx));

	isEqual(
		{ 0x0a, 0x01, 0x01 },
		AmfObject("foo", true, false).serialize(ctx));
}

TEST(ObjectSerialization, SelfReference) {
	AmfItemPtr ptr(AmfObject("", true, false));
	ptr.as<AmfObject>().dynamicProperties["f"] = ptr;

	SerializationContext ctx;
	isEqual({
		0x0a, 0x0b, 0x01,
		0x03, 0x66,
		0x0a, 0x00,
		0x01
	}, ptr->serialize(ctx));
}

TEST(ObjectSerialization, ReferenceOrder) {
	AmfObject obj("", true, false);
	AmfByteArray ba(v8 { 1 });
	obj.addDynamicProperty("a", ba);
	obj.addDynamicProperty("b", ba);

	SerializationContext ctx;
	isEqual({
		0x0a, 0x0b, 0x01,
		0x03, 0x61, 0x0c, 0x03, 0x01,
		0x03, 0x62, 0x0c, 0x02, // 0x00 = the outer object, 0x02 = bytearray
		0x01
	}, obj.serialize(ctx));
}

TEST(ObjectSerialization, DuplicateSealedProperties) {
	// Deserialize an object with a duplicate sealed property name.
	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x62, 0x03, 0x62,
		0x02, 0x03
	};

	auto it = data.cbegin();
	DeserializationContext ctx;
	AmfObject o = AmfObject::deserialize(it, data.cend(), ctx);

	// Serialize it again, verify the attribute name is only contained once and
	// the value is the last occurence.
	isEqual({
		0x0a, 0x13, 0x01,
		0x03, 0x62,
		0x03
	}, o);
}

TEST(ObjectEquality, EmptyObject) {
	AmfObject o1, o2;
	EXPECT_EQ(o1, o2);

	AmfObject o3("", false, false);
	EXPECT_EQ(o1, o3);

	AmfObject o4("foo", false, false);
	EXPECT_NE(o1, o4);

	AmfObject o5("", true, false);
	EXPECT_NE(o1, o5);

	AmfObject o6("", false, true);
	EXPECT_NE(o1, o6);
	EXPECT_NE(o5, o6);
}

TEST(ObjectEquality, DynamicProperties) {
	AmfObject o1("", true, false);
	AmfObject o2("", true, false);

	o1.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o2);
	o2.addDynamicProperty("foo", AmfString("qux"));
	EXPECT_NE(o1, o2);
	o2.addDynamicProperty("foo", AmfNull());
	EXPECT_NE(o1, o2);
	o2.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_EQ(o1, o2);

	AmfObject o3("foo", false, false);
	o3.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o3);

	AmfObject o4("", true, false);
	o4.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_EQ(o1, o4);

	AmfObject o5("", false, true);
	o5.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o5);

	AmfObject o6("", false, false);
	o5.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o6);

	o1.addDynamicProperty("qux", AmfNull());
	EXPECT_NE(o1, o2);
	o1.addDynamicProperty("qux", AmfUndefined());
	EXPECT_NE(o1, o2);
	o2.addDynamicProperty("qux", AmfNull());
	EXPECT_NE(o1, o2);
	o2.addDynamicProperty("qux", AmfUndefined());
	EXPECT_EQ(o1, o2);
}

TEST(ObjectEquality, DynamicPropertiesNonDynamicObject) {
	AmfObject o1, o2;
	o1.addDynamicProperty("foo", AmfUndefined());
	EXPECT_EQ(o1, o2);

	o2.addDynamicProperty("qux", AmfInteger(1));
	EXPECT_EQ(o1, o2);

	o1.addSealedProperty("foo", AmfInteger(1));
	EXPECT_NE(o1, o2);
}

TEST(ObjectEquality, SealedProperties) {
	AmfObject o1, o2;

	o1.addSealedProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o2);
	o2.addSealedProperty("foo", AmfString("qux"));
	EXPECT_NE(o1, o2);
	o2.addSealedProperty("foo", AmfNull());
	EXPECT_NE(o1, o2);
	o2.addSealedProperty("foo", AmfString("bar"));
	EXPECT_EQ(o1, o2);

	AmfObject o3("foo", false, false);
	o3.addSealedProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o3);

	AmfObject o4("", true, false);
	o4.addSealedProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o4);

	AmfObject o5("", false, true);
	o5.addSealedProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o5);

	o1.addSealedProperty("qux", AmfNull());
	EXPECT_NE(o1, o2);
	o1.addSealedProperty("qux", AmfUndefined());
	EXPECT_NE(o1, o2);
	o2.addSealedProperty("qux", AmfNull());
	EXPECT_NE(o1, o2);
	o2.addSealedProperty("qux", AmfUndefined());
	EXPECT_EQ(o1, o2);
}

TEST(ObjectEquality, CheckOnlyPropsInTraits) {
	AmfObject o1, o2;

	o1.addSealedProperty("foo", AmfInteger(1));
	o2.sealedProperties["foo"] = AmfItemPtr(AmfInteger(1));
	EXPECT_NE(o1, o2);

	o2.addSealedProperty("foo", AmfInteger(1));
	EXPECT_EQ(o1, o2);
}

TEST(ObjectEquality, DynamicAndSealed) {
	AmfObject o1, o2;

	o1.addSealedProperty("foo", AmfString("bar"));
	o2.addDynamicProperty("foo", AmfString("bar"));
	EXPECT_NE(o1, o2);

	o1.addDynamicProperty("foo", AmfString("bar"));
	o2.addSealedProperty("foo", AmfString("bar"));
	EXPECT_EQ(o1, o2);
}

TEST(ObjectEquality, MixedTypes) {
	AmfObject o;
	AmfString s("foo");
	AmfNull n;
	AmfUndefined u;

	EXPECT_NE(o, s);
	EXPECT_NE(o, n);
	EXPECT_NE(o, u);
}

TEST(ObjectDeserialization, EmptyDynamicAnonymousObject) {
	deserialize(AmfObject("", true, false), { 0x0a, 0x0b, 0x01, 0x01 });
	deserialize(AmfObject("", true, false), { 0x0a, 0x0b, 0x01, 0x01, 0x0a }, 1);
}

TEST(ObjectDeserialization, EmptyNamedDynamicObject) {
	deserialize(AmfObject("foo", true, false), { 0x0a, 0x0b, 0x07, 0x66, 0x6f,
		0x6f, 0x01 });
	deserialize(AmfObject("foo", true, false), { 0x0a, 0x0b, 0x07, 0x66, 0x6f,
		0x6f, 0x01, 0xff, 0xff, 0xff }, 3);
}

TEST(ObjectDeserialization, DynamicAnonymousObject) {
	AmfObject obj("", true, false);
	obj.addDynamicProperty("foo", AmfString("bar"));

	v8 data {
		0x0a, 0x0b, 0x01, 0x07, 0x66, 0x6f, 0x6f, 0x06, 0x07, 0x62, 0x61, 0x72, 0x01
	};

	deserialize(obj, data);
}

TEST(ObjectDeserialization, DynamicNamedObject) {
	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("foo", AmfString("foo"));

	v8 data {
		0x0a, 0x0b, 0x07, 0x66, 0x6f, 0x6f,
		0x00, // U29S-ref
		0x06, 0x00,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, EmptySealedAnonymousObject) {
	deserialize(AmfObject(), { 0x0a, 0x03, 0x01 });
}

TEST(ObjectDeserialization, SealedNamedObject) {
	AmfObject obj("foo", false, false);
	obj.addSealedProperty("foo", AmfString("foo"));

	v8 data {
		0x0a, 0x13, 0x07, 0x66, 0x6f, 0x6f,
		0x00,
		0x06, 0x00
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, SealedDynamicProps) {
	AmfObject obj("", true, false);
	obj.addDynamicProperty("foo", AmfInteger(1));
	obj.addSealedProperty("bar", AmfUndefined());

	v8 data {
		0x0a, 0x1b, 0x01,
		0x07, 0x62, 0x61, 0x72,
		0x00,
		0x07, 0x66, 0x6f, 0x6f,
		0x04, 0x01,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, MultipleSealedProperties) {
	AmfObject obj("", false, false);
	obj.addSealedProperty("b", AmfBool(false));
	obj.addSealedProperty("a", AmfBool(true));

	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x62, 0x03, 0x61,
		0x02, 0x03
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, DuplicateSealedProperties) {
	AmfObject obj("", false, false);
	obj.addSealedProperty("b", AmfBool(true));

	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x62, 0x03, 0x62,
		0x02, 0x03
	};

	auto it = data.cbegin();
	DeserializationContext ctx;
	AmfObject o = AmfObject::deserialize(it, data.cend(), ctx);

	// Objects should *not* compare equal, since the deserialized object's
	// traits contain *two* (identical) attribute names.
	EXPECT_NE(o, obj);
	// Check that the last value is kept.
	EXPECT_EQ(o.getSealedProperty<AmfBool>("b").value, true);
	EXPECT_EQ(it, data.cend());
}

TEST(ObjectDeserialization, MultipleSealedPropertiesTraitsReference) {
	AmfObject obj("", false, false);
	obj.addSealedProperty("b", AmfBool(false));
	obj.addSealedProperty("a", AmfBool(true));

	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x62, 0x03, 0x61,
		0x02, 0x03
	};
	DeserializationContext ctx;
	deserialize(obj, data, 0, &ctx);

	v8 data_ref {
		0x0a, 0x01,
		0x02, 0x03
	};
	deserialize(obj, data_ref, 0, &ctx);
}

TEST(ObjectDeserialization, DuplicateSealedPropertiesTraitsReference) {
	AmfObject obj("", false, false);
	obj.addSealedProperty("b", AmfBool(true));

	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x62, 0x03, 0x62,
		0x02, 0x03
	};

	auto it = data.cbegin();
	DeserializationContext ctx;
	AmfObject o1 = AmfObject::deserialize(it, data.cend(), ctx);

	// Objects should *not* compare equal, since the deserialized object's
	// traits contain *two* (identical) attribute names.
	EXPECT_NE(o1, obj);
	EXPECT_EQ(o1.getSealedProperty<AmfBool>("b").value, true);
	EXPECT_EQ(it, data.cend());

	// Test with traits reference.
	v8 data_ref {
		0x0a, 0x01,
		0x02, 0x03
	};
	deserialize(o1, data_ref, 0, &ctx);
}

TEST(ObjectDeserialization, MultipleProperties) {
	AmfObject obj("", true, false);
	obj.addDynamicProperty("1", AmfInteger(1));
	obj.addDynamicProperty("2", AmfInteger(2));
	obj.addDynamicProperty("3", AmfInteger(3));
	obj.addSealedProperty("zz", AmfString("zz"));
	obj.addSealedProperty("a", AmfByteArray(v8 { 1, 2, 3}));
	obj.addSealedProperty("b", AmfArray());
	obj.addSealedProperty("c", AmfBool(false));

	v8 data {
		0x0a, 0x4b, 0x01,
		0x05, 0x7a, 0x7a, 0x03, 0x61, 0x03, 0x62, 0x03, 0x63,
		0x06, 0x05, 0x7a, 0x7a,
		0x0c, 0x07, 0x01, 0x02, 0x03,
		0x09, 0x01, 0x01,
		0x02,
		0x03, 0x31, 0x04, 0x01,
		0x03, 0x32, 0x04, 0x02,
		0x03, 0x33, 0x04, 0x03,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, TraitsSetProperly) {
	// to test if the sealed properties are properly added to the traits, we
	// simply serialize the object again after deserialising, which should result
	// in output that's identical to the input

	// 2 sealed properties
	v8 data {
		0x0a, 0x23, 0x01,
		0x03, 0x61, 0x03, 0x62,
		0x02, 0x03
	};
	auto it = data.cbegin();
	AmfObject obj;
	ASSERT_NO_THROW({
		DeserializationContext ctx;
		obj = AmfObject::deserialize(it, data.cend(), ctx);
	});

	isEqual(data, obj);
}

TEST(ObjectDeserialization, NestedObject) {
	AmfObject obj("", true, false);
	obj.addDynamicProperty("foo", AmfObject("", false, false));

	v8 data {
		0x0a, 0x0b, 0x01,
		0x07, 0x66, 0x6f, 0x6f,
		0x0a, 0x03, 0x01,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, PropertyCache) {
	AmfObject obj("", true, false);
	AmfObject inner;
	obj.addDynamicProperty("foo", inner);
	obj.addSealedProperty("bar", inner);

	v8 data {
		0x0a, 0x1b, 0x01,
		0x07, 0x62, 0x61, 0x72,
		0x0a, 0x03, 0x01,
		0x07, 0x66, 0x6f, 0x6f,
		0x0a, 0x02,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, Context) {
	DeserializationContext ctx;
	AmfObject obj("", true, false);
	AmfObject obj2("", false, false);

	deserialize(obj, { 0x0a, 0x0b, 0x01, 0x01 }, 0, &ctx);
	deserialize(obj, { 0x0a, 0x00 }, 0, &ctx);
	deserialize(obj2, { 0x0a, 0x03, 0x01, 0xff }, 1, &ctx);
	deserialize(obj, { 0x0a, 0x00 }, 0, &ctx);
	deserialize(obj2, { 0x0a, 0x02 }, 0, &ctx);
}

TEST(ObjectDeserialization, ComplexNestedObject) {
	AmfObject obj("", true, false);
	AmfObject path("", true, false);
	AmfObject o25("", true, false);
	AmfObject o26("", true, false);
	AmfArray a33;
	AmfArray a34;
	AmfArray a35;

	a33.insert("prototype_id", AmfString("13"));
	a34.insert("prototype_id", AmfString("13"));
	a35.insert("prototype_id", AmfString("11"));

	o25.addDynamicProperty("33", a33);
	o25.addDynamicProperty("34", a34);
	o25.addDynamicProperty("35", a35);

	o26.addDynamicProperty("33", a33);

	path.addDynamicProperty("25", o25);
	path.addDynamicProperty("26", o26);

	obj.addDynamicProperty("path", path);

	v8 data {
		// obj
		0x0a, 0x0b, 0x01,
			// UTF-8-vr "path"
			0x09, 0x70, 0x61, 0x74, 0x68,
			// path, traits ref
			0x0a, 0x01,
				// UTF-8-vr "25"
				0x05, 0x32, 0x35,
				// o25, traits ref
				0x0a, 0x01,
					// UTF-8-vr "33"
					0x05, 0x33, 0x33,
					// AmfArray a33
					0x09, 0x01,
						// UTF-8-vr "prototype_id"
						0x19, 0x70, 0x72, 0x6f, 0x74, 0x6f, 0x74, 0x79, 0x70, 0x65, 0x5f, 0x69, 0x64,
						// AmfString "13"
						0x06, 0x05, 0x31, 0x33,
					0x01,
					// UTF-8-vr "34"
					0x05, 0x33, 0x34,
					// AmfArray
					0x09, 0x01,
						// string-ref "prototype_id"
						0x06,
						// AmfString "13"
						0x06, 0x08,
					0x01,
					// UTF-8-vr "35"
					0x05, 0x33, 0x35,
					// AmfArray
					0x09, 0x01,
						// string-ref "prototype_id"
						0x06,
						// AmfString "11"
						0x06, 0x05, 0x31, 0x31,
					0x01,
				0x01,
				// UTF-8-vr "26"
				0x05, 0x32, 0x36,
				// o26 traits-ref
				0x0a, 0x01,
					// string-ref "33"
					0x04,
					// AmfArray
					0x09, 0x01,
						// string-ref "prototype_id"
						0x06,
						// AmfString "13"
						0x06, 0x08,
					0x01,
				0x01,
			0x01,
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, Externalizable) {
	auto ext = [] (v8::const_iterator&, v8::const_iterator, DeserializationContext&) -> AmfObject {
		return AmfObject("foobar", true, false);
	};
	Deserializer::externalDeserializers["asd"] = ext;

	v8 data { 0x0a, 0x07, 0x07, 0x61, 0x73, 0x64 };
	deserialize(AmfObject("foobar", true, false), data);
}

TEST(ObjectDeserialization, ExternalizableFromData) {
	auto ext = [] (v8::const_iterator& it, v8::const_iterator end, DeserializationContext& ctx) -> AmfObject {
		AmfString className = AmfString::deserializeValue(it, end, ctx);
		return AmfObject(className, false, false);
	};
	Deserializer::externalDeserializers["asd"] = ext;

	v8 data {
		0x0a, 0x07,
		0x07, 0x61, 0x73, 0x64,
		0x0b, 0x63, 0x6c, 0x61, 0x73, 0x73
	};
	deserialize(AmfObject("class", false, false), data);
}

TEST(ObjectDeserialization, MissingExternalDeserializer) {
	v8 data {
		0x0a, 0x07,
		0x07, 0x61, 0x73, 0x64,
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	ASSERT_THROW(AmfObject::deserialize(it, data.cend(), ctx), std::out_of_range);
}

TEST(ObjectDeserialization, ExternalizableInContext) {
	std::string name("foo");
	// first time this is called, it returns AmfObject("foo"), afterwards AmfObject("bar");
	auto ext = [&name] (v8::const_iterator&, v8::const_iterator, DeserializationContext&) -> AmfObject {
		auto ret = AmfObject(name, false, false);
		name = "bar";
		return ret;
	};

	Deserializer::externalDeserializers["asd"] = ext;

	DeserializationContext ctx;
	deserialize(AmfObject("foo", false, false), { 0x0a, 0x07, 0x07, 0x61, 0x73, 0x64 }, 0, &ctx);
	deserialize(AmfObject("foo", false, false), { 0x0a, 0x00 }, 0, &ctx);
	deserialize(AmfObject("bar", false, false), { 0x0a, 0x07, 0x07, 0x61, 0x73, 0x64 }, 0, &ctx);
	deserialize(AmfObject("bar", false, false), { 0x0a, 0x02 }, 0, &ctx);
	deserialize(AmfObject("foo", false, false), { 0x0a, 0x00 }, 0, &ctx);
}

TEST(ObjectDeserialization, TraitRefs) {
	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("foo", AmfNull());

	DeserializationContext ctx;
	deserialize(obj, {
		0x0a, 0x0b,
		0x07, 0x66, 0x6f, 0x6f,
		0x00,
		0x01,
		0x01
	}, 0, &ctx);
	deserialize(AmfObject("foo", true, false), { 0x0a, 0x01, 0x01 }, 0, &ctx);
}

TEST(ObjectDeserialization, ReferenceOrder) {
	AmfObject obj("", true, false);
	AmfByteArray ba(v8 { 1 });
	obj.addDynamicProperty("a", ba);
	obj.addDynamicProperty("b", ba);

	v8 data {
		0x0a, 0x0b, 0x01,
		0x03, 0x61, 0x0c, 0x03, 0x01,
		0x03, 0x62, 0x0c, 0x02, // 0x00 = the outer object, 0x02 = bytearray
		0x01
	};
	deserialize(obj, data);
}

TEST(ObjectDeserialization, SelfReference) {
	// Dynamic, non-externalizable Object containing property "f", which
	// points to itself.
	v8 data {
		0x0a, 0x0b, 0x01,
		0x03, 0x66,
		0x0a, 0x00,
		0x01
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfObject::deserializePtr(it, data.cend(), ctx);
	const AmfObject & d = ptr.as<AmfObject>();
	EXPECT_EQ(ptr.get(), d.dynamicProperties.at("f").get());
}

TEST(ObjectDeserialization, SelfReferenceIncorrectType) {
	v8 data {
		0x0a, 0x0b, 0x01,
		0x03, 0x66,
		0x09, 0x01, 0x01,
		0x03, 0x67,
		// Object with reference index pointing to array
		0x0a, 0x02,
		0x01
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	ASSERT_THROW(AmfObject::deserializePtr(it, data.cend(), ctx), std::invalid_argument);
}

TEST(ObjectDeserialization, DynamicObjectMissingEndMarker) {
	DeserializationContext ctx;
	v8 data {
		0x0a, 0x0b, 0x01, 0x07, 0x66, 0x6f, 0x6f, 0x06, 0x07, 0x62, 0x61, 0x72
	};

	auto it = data.cbegin();
	ASSERT_THROW(AmfObject::deserialize(it, data.cend(), ctx), std::out_of_range);
}

TEST(ObjectDeserialization, EmptyIterator) {
	v8 data { };
	auto it = data.cbegin();
	auto end = data.cend();
	DeserializationContext ctx;
	ASSERT_THROW(AmfObject::deserialize(it, end, ctx), std::invalid_argument);
}

TEST(ObjectDeserialization, InvalidMarker) {
	DeserializationContext ctx;
	v8 data { 0xff };
	auto it = data.cbegin();
	ASSERT_THROW(AmfObject::deserialize(it, data.cend(), ctx), std::invalid_argument);

	data = { 0x0b };
	it = data.cbegin();
	ASSERT_THROW(AmfObject::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

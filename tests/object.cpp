#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfbool.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"

TEST(ObjectSerializationTest, EmptyDynamicAnonymousObject) {
	AmfObject obj("", true, false);

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x01, // class-name "" (anonymous object)
		0x01  // end of object (UTF-8-empty)
	}, obj);
}

TEST(ObjectSerializationTest, DynamicAnonymousObject) {
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

TEST(ObjectSerializationTest, SealedAnonymousObject) {
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

TEST(ObjectSerializationTest, SealedNamedObject) {
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

TEST(ObjectSerializationTest, DynamicSealedNamedObject) {
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

TEST(ObjectSerializationTest, EmptySealedAnonymousObject) {
	AmfObject obj("", false, false);

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // 0b0011, U29O-traits, not dynamic, 0 sealed properties
		0x01 // class-name "" (anonymous object)
	}, obj);
}

TEST(ObjectSerializationTest, SerializeOnlyPropsInTraits) {
	AmfObject obj("", false, false);

	obj.addSealedProperty("sealedProp", AmfInteger(0x05ffeffe));

	// this should not be serialized as it's not part of the trait attributes
	obj.sealedProperties["unusedProp"] = std::shared_ptr<AmfItem>(new AmfString("unused"));

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

TEST(ObjectSerializationTest, NoDynamicPropOnSealedObject) {
	AmfObject obj;
	obj.addDynamicProperty("foo", AmfInteger(17));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // U29O-traits, not dynamic, 0 sealed properties
		0x01 // class name ""
		// no dynamic property
	}, obj);
}

TEST(ObjectSerializationTest, OverwriteProperties) {
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

TEST(ObjectSerializationTest, NonTraitCtor) {
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

TEST(ObjectSerializationTest, OnlySerializeDynamicPropsOnDynamicObjects) {
	// non-dynamic object
	AmfObject obj("", false, false);
	obj.addDynamicProperty("dynamicProp", AmfString("val"));

	isEqual(v8 {
		0x0a, // AMF_OBJECT
		0x03, // 0b0011, U29O-traits, not dynamic, 0 sealed properties
		0x01 // class-name "" (anonymous object)
	}, obj);
}

TEST(ObjectSerializationTest, Externalizable) {
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
		for (const auto& it : o->dynamicProperties) {
			v8 s = AmfString(it.first).serialize();
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

TEST(ObjectSerializationTest, ExternalizableThrowsWithoutExternalizer) {
	AmfObject obj("", true, true);
	ASSERT_THROW(obj.serialize(), std::bad_function_call);
}

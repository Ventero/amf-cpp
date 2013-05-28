#include "gtest/gtest.h"

#include "amf.hpp"
#include "types/amfarray.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfobject.hpp"

void isEqual(const std::vector<u8>& expected, const AmfArray& value) {
	v8 serialized = value.serialize();
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();
}

TEST(ArraySerializationTest, EmptyArray) {
	AmfArray array;

	isEqual(v8 { 0x09, 0x01, 0x01 }, array);
}

TEST(ArraySerializationTest, StrictIntArray) {
	AmfInteger v0(0);
	AmfInteger v1(1);
	AmfInteger v2(2);
	AmfInteger v3(3);
	std::vector<AmfItem*> dense({{ &v0, &v1, &v2, &v3 }});
	AmfArray array(dense, {});

	isEqual(v8 {
		0x09,
		0x09,
		0x01,
		0x04, 0x00, 0x04, 0x01, 0x04, 0x02, 0x04, 0x03
	}, array);
}

TEST(ArraySerializationTest, StrictMixedArray) {
	AmfInteger v0(0);
	AmfString v1("value");
	AmfDouble v2(3.1);
	AmfObjectTraits traits("", true, false);
	AmfObject v3(traits);

	std::vector<AmfItem*> dense({{ &v0, &v1, &v2, &v3 }});
	AmfArray array(dense, {});

	isEqual(v8 {
		0x09,
		0x09,
		0x01,
		0x04, 0x00,
		0x06, 0x0b, 0x76, 0x61, 0x6c, 0x75, 0x65,
		0x05, 0x40, 0x08, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcd,
		0x0a, 0x0b, 0x01, 0x01
	}, array);
}

TEST(ArraySerializationTest, AssociativeOnlyArray) {
	AmfInteger v0(17);
	AmfInteger v1(0);

	std::map<std::string, AmfItem*> sparse;
	sparse["bar"] = &v0;
	sparse["foo"] = &v1;
	AmfArray array({}, sparse);

	isEqual(v8 {
		0x09, // AMF_ARRAY
		0x01, // 0 dense elements
		// assoc-values
		0x07, 0x62, 0x61, 0x72, // UTF-8-vr "bar"
		0x04, 0x11, // AmfInteger 17
		0x07, 0x66, 0x6f, 0x6f, // UTF-8-vr "foo"
		0x04, 0x00, // AmfInteger 0
		0x01 // end of assoc-values
	}, array);
}

TEST(ArraySerializationTest, AssociativeDenseArray) {
	AmfInteger v0(0xbeef);
	AmfString v1("foobar");

	std::map<std::string, AmfItem*> sparse;
	sparse["sparseVal"] = &v0;

	AmfArray array({ &v1 }, sparse);

	isEqual(v8 {
		0x09, // AMF_ARRAY
		0x03, // 1 dense element
		// assoc-values
		// UTF-8-vr "sparseVal"
		0x13, 0x73, 0x70, 0x61, 0x72, 0x73, 0x65, 0x56, 0x61, 0x6c,
		// AmfInteger 0xbeef
		0x04, 0x82, 0xfd, 0x6f,
		0x01, // end of assoc-values
		// dense elements
		// AmfString "foobar"
		0x06, 0x0d, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72,
	}, array);
}

TEST(ArraySerializationTest, AssociativeDenseArrayUtilityFunctions) {
	AmfInteger v0(0xbeef);
	AmfString v1("foobar");

	AmfArray array;
	array.push_back(&v1);
	array.insert("sparseVal", &v0);

	isEqual(v8 {
		0x09, // AMF_ARRAY
		0x03, // 1 dense element
		// assoc-values
		// UTF-8-vr "sparseVal"
		0x13, 0x73, 0x70, 0x61, 0x72, 0x73, 0x65, 0x56, 0x61, 0x6c,
		// AmfInteger 0xbeef
		0x04, 0x82, 0xfd, 0x6f,
		0x01, // end of assoc-values
		// dense elements
		// AmfString "foobar"
		0x06, 0x0d, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72,
	}, array);

}

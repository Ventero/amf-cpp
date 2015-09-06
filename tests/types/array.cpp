#include "amftest.hpp"

#include "amf.hpp"
#include "types/amfarray.hpp"
#include "types/amfbytearray.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfundefined.hpp"
#include "types/amfvector.hpp"

TEST(ArraySerializationTest, EmptyArray) {
	AmfArray array;

	isEqual(v8 { 0x09, 0x01, 0x01 }, array);
}

TEST(ArraySerializationTest, StrictIntArray) {
	AmfInteger v0(0);
	AmfInteger v1(1);
	AmfInteger v2(2);
	AmfInteger v3(3);
	std::vector<AmfInteger> dense {{ v0, v1, v2, v3 }};
	AmfArray array(dense);

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
	AmfObject v3("", true, false);

	AmfArray array;
	array.push_back(v0);
	array.push_back(v1);
	array.push_back(v2);
	array.push_back(v3);

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
	std::map<std::string, AmfInteger> sparse;
	sparse["bar"] = AmfInteger(17);
	sparse["foo"] = AmfInteger(0);
	AmfArray array(std::vector<AmfInteger> { }, sparse);

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
	std::map<std::string, AmfInteger> sparse;
	sparse["sparseVal"] = AmfInteger(0xbeef);

	AmfString v("foobar");
	std::vector<AmfString> vec { v };
	AmfArray array(vec, sparse);

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
	array.push_back(v1);
	array.insert("sparseVal", v0);

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

TEST(ArraySerializationTest, ArrayOfArrays) {
	AmfInteger v0(0xbeef);
	AmfString v1("foobar");

	AmfArray array;
	array.push_back(v0);
	array.push_back(v1);

	AmfArray outerArray;
	outerArray.push_back(array);
	outerArray.push_back(array);

	isEqual(v8 {
		// AMF_ARRAY
		0x09,
		// 2 dense elements
		0x05,
		// end of associative elements
		0x01,
			// first dense element, AMF_ARRAY
			0x09,
			// 2 dense elements
			0x05,
			// end of associative elements
			0x01,
				// AmfInteger 0xbeef
				0x04, 0x82, 0xfd, 0x6f,
				// AmfString "foobar"
				0x06, 0x0d, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72,
			// second dense element, AMF_ARRAY
			0x09,
			// reference to array with index 1
			0x02
	}, outerArray);
}

TEST(ArraySerializationTest, SelfReference) {
	SerializationContext ctx;
	AmfItemPtr ptr((AmfArray()));
	ptr.as<AmfArray>().associative["x"] = ptr;
	ptr.as<AmfArray>().dense.push_back(ptr);

	isEqual(v8 {
		0x09, 0x03,
			0x03, 0x78,
			0x09, 0x00,
			0x01,
			0x09, 0x00,
	}, ptr.as<AmfArray>(), &ctx);
}

TEST(ArraySerializationTest, ArrayReference) {
	SerializationContext ctx;
	AmfArray arr;
	AmfArray inner;
	inner.insert("x", AmfNull());
	arr.push_back(inner);
	arr.push_back(inner);

	isEqual(v8 {
		0x09, 0x05,
			0x01,
			0x09, 0x01, 0x03, 0x78, 0x01, 0x01,
			0x09, 0x02
	}, arr, &ctx);
}

TEST(ArraySerializationTest, ArrayReferenceOrder) {
	SerializationContext ctx;
	AmfItemPtr ptr((AmfArray()));
	ptr.as<AmfArray>().associative["x"] = ptr;
	ptr.as<AmfArray>().insert("y", AmfArray());
	ptr.as<AmfArray>().push_back(AmfArray());
	ptr.as<AmfArray>().dense.push_back(ptr);

	isEqual(v8 {
		0x09, 0x05,
			0x03, 0x78, 0x09, 0x00,
			0x03, 0x79, 0x09, 0x01, 0x01,
			0x01,
			0x09, 0x02,
			0x09, 0x00,
	}, ptr.as<AmfArray>(), &ctx);
}

TEST(ArraySerializationTest, Utf8VrReference) {
	SerializationContext ctx;
	AmfArray array;
	array.insert("x", AmfString("x"));

	isEqual(v8 { 0x09, 0x01, 0x03, 0x78, 0x06, 0x00, 0x01 }, array, &ctx);
	isEqual(v8 { 0x09, 0x00 }, array, &ctx);

	array.push_back(AmfUndefined());
	isEqual(v8 { 0x09, 0x03, 0x00, 0x06, 0x00, 0x01, 0x00 }, array, &ctx);
}

TEST(ArrayMember, Modify) {
	AmfInteger* v0 = new AmfInteger(0xbeef);
	AmfArray array;
	array.push_back(*v0);

	ASSERT_EQ(array.at<AmfInteger>(0), *v0);

	delete v0;
	ASSERT_EQ(array.at<AmfInteger>(0), AmfInteger(0xbeef));

	array.at<AmfInteger>(0).value = 0xbad;
	ASSERT_EQ(array.at<AmfInteger>(0), AmfInteger(0xbad));

	array.at<AmfInteger>(0) = AmfInteger(0x38);
	ASSERT_EQ(array.at<AmfInteger>(0), AmfInteger(0x38));
}

TEST(ArrayEquality, IntArray) {
	AmfArray a0;
	AmfArray a1;
	EXPECT_EQ(a0, a1);

	AmfArray a2(std::vector<AmfInteger> { 1, 2, 3 });
	AmfArray a3;
	a3.push_back(AmfInteger(1));
	a3.push_back(AmfInteger(2));
	a3.push_back(AmfInteger(3));
	EXPECT_EQ(a2, a3);
	a3.insert("foo", AmfInteger(0));
	EXPECT_NE(a2, a3);
	a2.insert("foo", AmfInteger(0));
	EXPECT_EQ(a2, a3);

	AmfArray a4(std::vector<AmfInteger> { 1, 2, 3 },
		std::map<std::string, AmfInteger> { { "foo", 0 } });
	EXPECT_EQ(a2, a4);
}

TEST(ArrayEquality, MixedArray) {
	AmfArray a0, a1;
	a0.push_back(AmfInteger(1));
	a1.push_back(AmfInteger(1));
	a0.push_back(AmfString("foo"));
	a1.push_back(AmfString("foo"));
	EXPECT_EQ(a0, a1);
}

TEST(ArrayEquality, NestedArray) {
	AmfArray a0, a1, i0;
	i0.push_back(AmfInteger(1));
	i0.insert("qux", AmfString("foo"));
	a0.push_back(i0);
	a1.push_back(i0);
	a0.insert("a", i0);
	a1.insert("a", i0);
	EXPECT_EQ(a0, a1);
}

TEST(ArrayEquality, SparseArray) {
	AmfArray a0 { std::vector<AmfInteger> {}, std::map<std::string, AmfString> {
		{ "foo", "bar" },
		{ "qux", "quux" }
	} };
	AmfArray a1;
	a1.insert("foo", AmfString("bar"));
	a1.insert("qux", AmfString("quux"));
	EXPECT_EQ(a0, a1);
}

TEST(ArrayEquality, MixedTypes) {
	AmfArray a0(std::vector<AmfInteger> { 1, 2, 3});
	AmfArray a1(std::vector<AmfDouble> { 1, 2, 3});
	EXPECT_NE(a0, a1);

	AmfVector<int> v1 { { 1, 2, 3 } };
	EXPECT_NE(a0, v1);

	AmfVector<unsigned int> v2 { { 1, 2, 3} };
	EXPECT_NE(a0, v2);
}

// Stack overflow when comparing ptr and ptr2
// should use ecmascript equality semantics instead (reference comparison
// for object types, value comparison for primitives)?
TEST(ArrayEquality, DISABLED_SelfReference) {
	AmfItemPtr ptr((AmfArray()));
	ptr.as<AmfArray>().dense.push_back(ptr);

	EXPECT_EQ(ptr, ptr);

	AmfItemPtr ptr2((AmfArray()));
	ptr2.as<AmfArray>().dense.push_back(ptr2);

	EXPECT_EQ(ptr, ptr2);
}

static void deserializesTo(AmfArray value, const v8& data, int left = 0,
	DeserializationContext* ctx = nullptr) {
	deserialize(value, data, left, ctx);
}

TEST(ArrayDeserialization, EmptyArray) {
	deserializesTo(AmfArray(), { 0x09, 0x01, 0x01 }, 0);
	deserializesTo(AmfArray(), { 0x09, 0x01, 0x01, 0x01 }, 1);
	deserializesTo(AmfArray(), { 0x09, 0x01, 0x01, 0x09 }, 1);
	deserializesTo(AmfArray(), { 0x09, 0x01, 0x01, 0x09, 0x00, 0x00 }, 3);
}

TEST(ArrayDeserialization, IntDenseArray) {
	AmfArray a(std::vector<AmfInteger> { 1, 2, 3, });
	v8 data {
		0x09, 0x07, 0x01, 0x04, 0x01, 0x04, 0x02, 0x04, 0x03
	};
	deserializesTo(a, data, 0);
}

TEST(ArrayDeserialization, MixedDenseArray) {
	AmfArray a;
	a.push_back(AmfInteger(1));
	a.push_back(AmfString("foo"));
	a.push_back(AmfByteArray(v8 { 1, 2, 3 }));

	v8 data {
		0x09, 0x07, 0x01, 0x04, 0x01, 0x06, 0x07, 0x66, 0x6f, 0x6f,
		0x0c, 0x07, 0x01, 0x02, 0x03,
		0xff
	};

	deserializesTo(a, data, 1);
}

TEST(ArrayDeserialization, MixedDenseArrayWithObject) {
	AmfArray a;
	a.push_back(AmfInteger(1));
	a.push_back(AmfString("foo"));
	a.push_back(AmfObject("", true, false));

	v8 data {
		0x09, 0x07, 0x01, 0x04, 0x01, 0x06, 0x07, 0x66, 0x6f, 0x6f, 0x0a, 0x0b,
		0x01, 0x01,
		0xff
	};

	deserializesTo(a, data, 1);
}

TEST(ArrayDeserialization, SparseArray) {
	AmfArray a;
	a.insert("foo", AmfInteger(1));
	a.insert("bar", AmfString("asd"));

	v8 data {
		0x09, 0x01, 0x07, 0x66, 0x6f, 0x6f, 0x04, 0x01, 0x07, 0x62, 0x61, 0x72,
		0x06, 0x07, 0x61, 0x73, 0x64, 0x01
	};
	deserializesTo(a, data, 0);
}

TEST(ArrayDeserialization, SparseArrayStringCache) {
	AmfArray a;
	a.insert("foo", AmfInteger(1));
	a.insert("bar", AmfString("foo"));

	v8 data {
		0x09, 0x01, 0x07, 0x66, 0x6f, 0x6f, 0x04, 0x01, 0x07, 0x62, 0x61, 0x72,
		0x06, 0x00, 0x01
	};
	deserializesTo(a, data, 0);
}

TEST(ArrayDeserialization, MixedDenseSpareArray) {
	AmfArray a;
	a.push_back(AmfUndefined());
	a.push_back(AmfNull());
	a.push_back(AmfArray());
	a.push_back(AmfString("foo"));
	a.insert("foo", AmfString("qux"));
	a.insert("bar", AmfArray());

	v8 data {
		0x09, 0x09, 0x07, 0x66, 0x6f, 0x6f, 0x06, 0x07, 0x71, 0x75, 0x78, 0x07,
		0x62, 0x61, 0x72, 0x09, 0x01, 0x01, 0x01, 0x00, 0x01, 0x09, 0x01, 0x01,
		0x06, 0x00
	};
	deserializesTo(a, data, 0);
}

TEST(ArrayDeserialization, ArrayOfArrays) {
	AmfInteger v0(0xbeef);
	AmfString v1("foobar");

	AmfArray array;
	array.push_back(v0);
	array.push_back(v1);

	AmfArray outerArray;
	outerArray.push_back(array);
	outerArray.push_back(array);

	v8 data {
		0x09,
		0x05,
		0x01,
			0x09,
			0x05,
			0x01,
				0x04, 0x82, 0xfd, 0x6f,
				0x06, 0x0d, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72,
			0x09,
			0x05,
			0x01,
				0x04, 0x82, 0xfd, 0x6f,
				0x06, 0x0d, 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72
	};

	deserializesTo(outerArray, data, 0);
}

TEST(ArrayDeserialization, ObjectCache) {
	DeserializationContext ctx;
	deserializesTo(AmfArray(), { 0x09, 0x01, 0x01 }, 0, &ctx);
	deserializesTo(AmfArray(), { 0x09, 0x00, 0x01 }, 1, &ctx);

	AmfArray a;
	a.push_back(AmfInteger(1));
	deserializesTo(a, { 0x09, 0x03, 0x01, 0x04, 0x01 }, 0, &ctx);
	deserializesTo(a, { 0x09, 0x02 }, 0, &ctx);
	deserializesTo(AmfArray(), { 0x09, 0x00, 0x01, 0x02, 0x03 }, 3, &ctx);
}

TEST(ArrayDeserialization, ReferenceIndexOrder) {
	AmfArray a;
	AmfByteArray b(v8 {1, 2, 3});
	a.push_back(b);
	a.push_back(b);

	v8 data {
		0x09, 0x05, 0x01,
			0x0c, 0x07, 0x01, 0x02, 0x03,
			// index 0x00 = the outer array, 0x02 = the bytearray
			0x0c, 0x02
	};
	deserializesTo(a, data, 0);
}


TEST(ArrayDeserialization, NotEnoughBytes) {
	DeserializationContext ctx;

	v8 data = { 0x09 };
	auto it = data.cbegin();
	ASSERT_THROW(AmfArray::deserialize(it, data.cend(), ctx), std::out_of_range);

	v8 data2 = { 0x09, 0x01 };
	it = data2.cbegin();
	ASSERT_THROW(AmfArray::deserialize(it, data2.cend(), ctx), std::out_of_range);

	v8 data3 = { 0x09, 0x03, 0x01 };
	it = data3.cbegin();
	ASSERT_THROW(AmfArray::deserialize(it, data3.cend(), ctx), std::out_of_range);
}

TEST(ArrayDeserialization, InvalidMarker) {
	v8 data = { 0x0a };
	auto it = data.cbegin();
	DeserializationContext ctx;
	ASSERT_THROW(AmfArray::deserialize(it, data.cend(), ctx), std::invalid_argument);
}


TEST(ArrayDeserialization, SelfReference) {
	// Array containing 3 integers in dense part and a reference to itself
	// in the associative part.
	v8 data {
		0x09,
		0x07,
		0x07, 0x66, 0x6f, 0x6f,
		0x09, 0x00,
		0x01,
		0x04, 0x01,
		0x04, 0x02,
		0x04, 0x03
	};


	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfArray::deserializePtr(it, data.cend(), ctx);
	AmfArray & d = ptr.as<AmfArray>();

	EXPECT_EQ(1, d.at<AmfInteger>(0));
	EXPECT_EQ(2, d.at<AmfInteger>(1));
	EXPECT_EQ(3, d.at<AmfInteger>(2));

	EXPECT_EQ(ptr.get(), d.associative.at("foo").get());
	const AmfArray & ref = d.associative.at("foo").as<AmfArray>();

	// Verify changes to the outer array are reflected in the inner one.
	d.push_back(AmfInteger(4));
	EXPECT_EQ(4, d.at<AmfInteger>(3));
	EXPECT_EQ(4, ref.at<AmfInteger>(3));
}

TEST(ArrayDeserialization, ArraySelfReferenceStackOverflow) {
	v8 data {
		0x09, 0x05, 0x01,
		0x0a, 0x03, 0x01,
		0x09, 0x00,
	};

	// Flash chokes with a stack overflow on this one.
	DeserializationContext ctx;
	auto it = data.cbegin();
	AmfItemPtr ptr = AmfArray::deserializePtr(it, data.cend(), ctx);

	AmfArray & d = ptr.as<AmfArray>();

	AmfObject empty("", false, false);
	EXPECT_EQ(empty, d.at<AmfObject>(0));
	EXPECT_EQ(ptr.get(), d.dense.at(1).get());
}

TEST(ArrayDeserialization, ArraySelfReferenceInvalidType) {
	v8 data {
		0x09, 0x05, 0x01,
		0x0a, 0x03, 0x01,
		// object-typed reference to array
		0x0a, 0x00,
	};

	DeserializationContext ctx;
	auto it = data.cbegin();
	ASSERT_THROW(AmfArray::deserialize(it, data.cend(), ctx), std::invalid_argument);
}

TEST(ArrayDeserialization, Utf8VrReference) {
	v8 data { 0x09, 0x01, 0x03, 0x78, 0x06, 0x00, 0x01 };

	AmfArray array;
	array.insert("x", AmfString("x"));

	DeserializationContext ctx;
	deserializesTo(array, data, 0, &ctx);
	deserializesTo(array, { 0x09, 0x01, 0x00, 0x06, 0x00, 0x01 }, 0, &ctx);
}

TEST(ArrayDeserialization, ArrayReferenceOrder) {
	AmfItemPtr ptr((AmfArray()));
	ptr.as<AmfArray>().associative["x"] = ptr;
	ptr.as<AmfArray>().insert("y", AmfArray());
	ptr.as<AmfArray>().push_back(AmfArray());
	ptr.as<AmfArray>().dense.push_back(ptr);

	v8 data {
		0x09, 0x05,
			0x03, 0x78, 0x09, 0x00,
			0x03, 0x79, 0x09, 0x01, 0x01,
			0x01,
			0x09, 0x02,
			0x09, 0x00,
	};

	DeserializationContext ctx;
	auto begin = data.cbegin();
	AmfItemPtr deserialized = AmfArray::deserializePtr(begin, data.cend(), ctx);
	AmfArray const & arr = deserialized.as<AmfArray>();
	ASSERT_EQ(arr.associative.at("x"), deserialized);
	ASSERT_EQ(arr.at<AmfArray>("y"), AmfArray());
	ASSERT_EQ(arr.at<AmfArray>(0), AmfArray());
	ASSERT_EQ(arr.dense.at(1), deserialized);
	ASSERT_EQ(arr.associative.at("y"), arr.dense.at(0));
}

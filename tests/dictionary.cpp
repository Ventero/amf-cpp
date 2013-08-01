#include "amftest.hpp"
#include "gtest/gtest-printers.h"

#include "amf.hpp"
#include "types/amfarray.hpp"
#include "types/amfbool.hpp"
#include "types/amfdictionary.hpp"
#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "types/amfobject.hpp"
#include "types/amfstring.hpp"
#include "types/amfvector.hpp"

void isEqual(const v8& expected, const AmfDictionary& value) {
	v8 serialized = value.serialize();
	ASSERT_EQ(expected, serialized) << "Expected length " << expected.size()
	                                << ", got " << serialized.size();
}

std::vector<v8>::iterator findElement(const v8::const_iterator& start, std::vector<v8>& parts) {
	return std::find_if(parts.begin(), parts.end(), [=] (const v8& part) {
		return std::equal(part.begin(), part.end(), start);
	});
}

void consistsOf(std::vector<v8> parts, const v8& data) {
	auto it = data.begin();
	while (it != data.end()) {
		if (parts.empty()) {
			FAIL() << "Unexpected elements left in data: "
			       << testing::PrintToString(v8(it, data.end())) << std::endl;
			return;
		}

		std::vector<v8>::iterator found = findElement(it, parts);
		if (found == parts.end()) {
			ADD_FAILURE() << "None of the following values could be found at position "
		                << std::distance(data.begin(), it) << " of data:\n"
										<< testing::PrintToString(v8(it, data.end())) << "\n";
			for (auto part : parts)
				std::cerr << testing::PrintToString(part) << "\n";

			return;
		}

		it += found->size();
		parts.erase(found);
	}

	SUCCEED();
}


TEST(DictionarySerializationTest, IntegerKeys) {
	AmfDictionary d(false, false);
	d[AmfInteger(3)] = AmfBool(false).serialize();

	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x04, 0x03, // AmfInteger 3
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(false, false);
	d.insert(AmfInteger(-16384), AmfString("foo"));
	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x04, 0xFF, 0xFF, 0xC0, 0x00,
		0x06, 0x07, 0x66, 0x6F, 0x6F
	}, d);
}

TEST(DictionarySerializationTest, BooleanKeys) {
	AmfDictionary d(false, false);
	d.insert(AmfBool(true), AmfBool(false));

	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x03, // AmfBool true
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(false, true);
	d.insert(AmfBool(false), AmfBool(true));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x02, // AmfBool false
		0x03 // AmfBool true
	}, d);
}

TEST(DictionarySerializationTest, NumberKeys) {
	AmfDictionary d(false, true);
	d.insert(AmfDouble(-0.5), AmfInteger(3));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x05, 0xBF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // AmfDouble -0.5
		0x04, 0x03 // AmfInteger 3
	}, d);

	d = AmfDictionary(false, false);
	d.insert(AmfDouble(1.2345678912345e+35), AmfArray());
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x05, 0x47, 0x37, 0xC6, 0xE3, 0xC0, 0x32, 0xF7, 0x20, // AmfDouble 1.2345678912345e+35
		0x09, 0x01, 0x01 // empty AmfArray
	}, d);
}

TEST(DictionarySerializationTest, IntegerAsStringKeys) {
	AmfDictionary d(true, false);
	d[AmfInteger(3)] = AmfBool(false).serialize();

	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x03, 0x33, // AmfInteger 3 serialized as AmfString "3"
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(true, false);
	d.insert(AmfInteger(-16384), AmfString("foo"));
	isEqual(v8 {
		0x11, // AMF_DICTIONARY
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x0D, 0x2D, 0x31, 0x36, 0x33, 0x38, 0x34,
		0x06, 0x07, 0x66, 0x6F, 0x6F
	}, d);
}

TEST(DictionarySerializationTest, BooleanAsStringKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfBool(true), AmfBool(false));

	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		0x06, 0x09, 0x74, 0x72, 0x75, 0x65, // AmfString "true"
		0x02 // AmfBool false
	}, d);

	d = AmfDictionary(true, true);
	d.insert(AmfBool(false), AmfBool(true));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65, // AmfString "false"
		0x03 // AmfBool true
	}, d);
}

TEST(DictionarySerializationTest, NumberAsStringKeys) {
	AmfDictionary d(true, true);
	d.insert(AmfDouble(-0.5), AmfInteger(3));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x06, 0x09, 0x2D, 0x30, 0x2E, 0x35, // AmfString "-0.5"
		0x04, 0x03 // AmfInteger 3
	}, d);

	d = AmfDictionary(true, false);
	d.insert(AmfDouble(1.2345678912345e+35), AmfArray());
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// AmfString "1.2345678912345e+35"
		0x06, 0x27, 0x31, 0x2e, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x65, 0x2b, 0x33, 0x35,
		0x09, 0x01, 0x01 // empty AmfArray
	}, d);
}

TEST(DictionarySerializationTest, MultipleKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfInteger(3), AmfBool(false));
	d.insert(AmfInteger(-16384), AmfString("foo"));
	consistsOf(std::vector<v8> {
		{ // header
			0x11, // AMF_DICTIONARY
			0x05, // 2 elements
			0x00, // no weak keys
		},
		{ // "3" = false
			0x06, 0x03, 0x33,
			0x02,
		},
		{ // "-16384" = "foo"
			0x06, 0x0D, 0x2D, 0x31, 0x36, 0x33, 0x38, 0x34,
			0x06, 0x07, 0x66, 0x6F, 0x6F
		}
	}, d.serialize());
}

TEST(DictionarySerializationTest, Clear) {
	AmfDictionary d(false, true);
	d.insert(AmfBool(true), AmfInteger(17));
	d.clear();

	isEqual(v8 {
		0x11,
		0x01, // no elements
		0x01  // weak keys
	}, d);

	d.insert(AmfBool(false), AmfDouble(17.0));
	isEqual(v8 {
		0x11,
		0x03, // 1 element
		0x01, // weak keys
		0x02, // AmfBool false
		0x05, 0x40, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // AmfDouble 17.0
	}, d);
}

TEST(DictionarySerializationTest, ComplexObjectKeys) {
	AmfDictionary d(false);
	d.insert(AmfArray(), AmfArray());
	d.insert(AmfArray(std::vector<AmfInteger> { 1 }), AmfObject("", true, false));

	consistsOf(std::vector<v8> {
		{ // header
			0x11,
			0x05, // 2 elements
			0x00  // no weak keys
		},
		{ // [] = []
			0x09, 0x01, 0x01, // empty array
			0x09, 0x01, 0x01 // empty array
		},
		{ // [1] = {}
			0x09, 0x03, 0x01, 0x04, 0x01, // AmfArray [1]
			0x0a, 0x0b, 0x01, 0x01 // empty dynamic anonymous object
		}
	}, d.serialize());


	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("prop", AmfString("val"));

	AmfVector<int> vec { { 1, 2, 3 }, false };

	d = AmfDictionary(false);
	d.insert(obj, vec);
	isEqual({
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// key
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x07, 0x66, 0x6f, 0x6f, // class-name "foo"
		// dynamic-member
		0x09, 0x70, 0x72, 0x6f, 0x70, // UTF-8-vr "prop"
		0x06, 0x07, 0x76, 0x61, 0x6c, // AmfString "val"
		0x01, // end of object (UTF-8-empty)
		// value
		0x0d, 0x07, 0x00, // AmfVector<int> with 3 elements
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03
	}, d);
}

TEST(DictionarySerializationTest, NumberAsStringsDoesntAffectObjects) {
	AmfDictionary d(true);
	d.insert(AmfArray(), AmfArray());
	d.insert(AmfArray(std::vector<AmfInteger> { 1 }), AmfObject("", true, false));

	consistsOf(std::vector<v8> {
		{ // header
			0x11,
			0x05, // 2 elements
			0x00  // no weak keys
		},
		{ // [] = []
			0x09, 0x01, 0x01, // empty array
			0x09, 0x01, 0x01 // empty array
		},
		{ // [1] = {}
			0x09, 0x03, 0x01, 0x04, 0x01, // AmfArray [1]
			0x0a, 0x0b, 0x01, 0x01 // empty dynamic anonymous object
		}
	}, d.serialize());


	AmfObject obj("foo", true, false);
	obj.addDynamicProperty("prop", AmfString("val"));

	AmfVector<int> vec { { 1, 2, 3 }, false };

	d = AmfDictionary(true);
	d.insert(obj, vec);
	isEqual({
		0x11,
		0x03, // 1 element
		0x00, // no weak keys
		// key
		0x0a, // AMF_OBJECT
		0x0b, // U29O-traits | dynamic, 0 sealed properties
		0x07, 0x66, 0x6f, 0x6f, // class-name "foo"
		// dynamic-member
		0x09, 0x70, 0x72, 0x6f, 0x70, // UTF-8-vr "prop"
		0x06, 0x07, 0x76, 0x61, 0x6c, // AmfString "val"
		0x01, // end of object (UTF-8-empty)
		// value
		0x0d, 0x07, 0x00, // AmfVector<int> with 3 elements
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03
	}, d);
}

TEST(DictionarySerializationTest, MultiByteLength) {
	AmfDictionary d(true, false);
	for(int i = 0; i < 300; ++i)
		d.insert(AmfInteger(i), AmfInteger(i));

	// for simplicity, only test header and total length
	v8 expected { 0x11, 0x84, 0x59, 0x00 };
	v8 actual(d.serialize());
	v8 header(actual.begin(), actual.begin() + 4);

	ASSERT_EQ(2166, actual.size());
	ASSERT_EQ(expected, header);
}

TEST(DictionarySerializationTest, OverwriteKeys) {
	AmfDictionary d(true, false);
	d.insert(AmfBool(false), AmfInteger(3));

	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65,
		0x04, 0x03
	}, d);

	d.insert(AmfBool(false), AmfString("foo"));
	isEqual({
		0x11, 0x03, 0x00,
		0x06, 0x0B, 0x66, 0x61, 0x6C, 0x73, 0x65,
		0x06, 0x07, 0x66, 0x6f, 0x6f
	}, d);
}

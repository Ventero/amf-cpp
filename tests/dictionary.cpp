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

TEST(DictionarySerializationTest, BooleanKeys) {
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

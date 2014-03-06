#include <vector>

#include "gtest/gtest.h"

#include "amf.hpp"
#include "deserializationcontext.hpp"

#include "types/amfitem.hpp"

using namespace amf;

static inline void isEqual(const std::vector<u8>& expected, const v8& serialized) {
	ASSERT_EQ(expected, serialized)
		<< "Expected length " << expected.size()
		<< ", got " << serialized.size();
}

static inline void isEqual(const std::vector<u8>& expected, const AmfItem& value) {
	v8 serialized = value.serialize();
	isEqual(expected, serialized);
}

template<typename T, typename V>
void deserialize(V expected, const v8& data, int expectedLeft = 0,
		DeserializationContext* ctx = nullptr) {
	SCOPED_TRACE(::testing::PrintToString(expected) + " = " + ::testing::PrintToString(data));

	std::unique_ptr<DeserializationContext> dummy;
	if (!ctx) {
		dummy.reset(new DeserializationContext());
		ctx = dummy.get();
	}

	auto it = data.begin();
	decltype(std::declval<T>().value) value;
	ASSERT_NO_THROW({
		T i = T::deserialize(it, data.end(), *ctx);
		value = i.value;
	});

	ASSERT_EQ(expectedLeft, data.end() - it)
		<< "Expected " << expectedLeft
		<< " bytes left, got " << (data.end() - it)
		<< " bytes left";
	ASSERT_EQ(expected, value)
		<< "Expected value " << ::testing::PrintToString(expected)
		<< ", got " << ::testing::PrintToString(value);
}

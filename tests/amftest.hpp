#include <stdexcept>
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

template<typename T>
void deserialize(T expected, const v8& data, int expectedLeft = 0,
		DeserializationContext* ctx = nullptr) {
	SCOPED_TRACE(::testing::PrintToString(expected) + " = " + ::testing::PrintToString(data));

	std::unique_ptr<DeserializationContext> dummy;
	if (!ctx) {
		dummy.reset(new DeserializationContext());
		ctx = dummy.get();
	}

	auto it = data.begin();
	try {
		T i = T::deserialize(it, data.end(), *ctx);
		ASSERT_EQ(expected, i);
	} catch(std::exception& e) {
		FAIL() << "Deserialization threw exception:\n"
		       << e.what() ;
	}

	ASSERT_EQ(expectedLeft, data.end() - it)
		<< "Expected " << expectedLeft
		<< " bytes left, got " << (data.end() - it)
		<< " bytes left";
}

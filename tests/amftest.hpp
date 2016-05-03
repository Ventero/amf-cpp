#include <stdexcept>
#include <vector>

#include "gtest/gtest.h"

#include "amf.hpp"
#include "serializationcontext.hpp"

#include "types/amfitem.hpp"

using namespace amf;

static inline void isEqual(const std::vector<u8>& expected, const v8& serialized) {
	ASSERT_EQ(expected, serialized)
		<< "Expected length " << expected.size()
		<< ", got " << serialized.size();
}

static inline void isEqual(
	const std::vector<u8>& expected,
	const AmfItem& value,
	SerializationContext * ctx = nullptr) {

	std::unique_ptr<SerializationContext> dummy;
	if (!ctx) {
		dummy.reset(new SerializationContext());
		ctx = dummy.get();
	}

	v8 serialized = value.serialize(*ctx);
	isEqual(expected, serialized);
}

template<typename T>
void deserialize(const T& expected, const v8& data, int expectedLeft = 0,
		SerializationContext* ctx = nullptr) {
	SerializationContext sctx;
	SCOPED_TRACE(::testing::PrintToString(expected.serialize(sctx)) + " = " + ::testing::PrintToString(data));

	std::unique_ptr<SerializationContext> dummy;
	if (!ctx) {
		dummy.reset(new SerializationContext());
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

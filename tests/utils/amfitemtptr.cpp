#include "amftest.hpp"

#include "types/amfdouble.hpp"
#include "types/amfinteger.hpp"
#include "utils/amfitemptr.hpp"

TEST(AmfItemPtrTest, Construction) {
	AmfItemPtr empty;
	EXPECT_EQ(nullptr, empty.get());

	const AmfInteger i(17);
	const AmfItemPtr ref(i);
	EXPECT_EQ(i, *ref);
	// Verify that it makes a copy.
	EXPECT_NE(&i, ref.get());

	AmfInteger * ip = new AmfInteger(42);
	AmfItemPtr ptr(ip);
	EXPECT_EQ(*ip, *ptr);
	// Verify that no copy is made.
	EXPECT_EQ(ip, ptr.get());
}

TEST(AmfItemPtrTest, AsReference) {
	AmfItemPtr ptr(new AmfInteger(33));

	EXPECT_EQ(AmfInteger(33), ptr.as<AmfInteger>());
	EXPECT_THROW(ptr.as<AmfDouble>(), std::bad_cast);
}

TEST(AmfItemPtrTest, AsPointer) {
	AmfInteger * ip = new AmfInteger(12);
	AmfItemPtr ptr(ip);
	EXPECT_EQ(ip, ptr.asPtr<AmfInteger>());
	EXPECT_EQ(nullptr, ptr.asPtr<AmfDouble>());

	// Const version.
	AmfDouble * dp = new AmfDouble(13.0);
	const AmfItemPtr cptr(dp);
	EXPECT_EQ(dp, cptr.asPtr<AmfDouble>());
	EXPECT_EQ(nullptr, cptr.asPtr<AmfInteger>());
}

TEST(AmfItemPtrTest, Equality) {
	AmfItemPtr i1(new AmfInteger(12));
	AmfItemPtr i2(new AmfInteger(12));
	AmfItemPtr d1(new AmfDouble(12.0));

	EXPECT_EQ(i1, i1);
	EXPECT_EQ(i1, i2);
	EXPECT_NE(i1, d1);
}

#include "amftest.hpp"

#include "deserializationcontext.hpp"
#include "types/amfinteger.hpp"
#include "types/amfnull.hpp"
#include "types/amfdouble.hpp"

TEST(DeserializationContextTest, String) {
	DeserializationContext ctx;

	ASSERT_THROW(ctx.getString(0), std::out_of_range);
	ASSERT_THROW(ctx.getString(1), std::out_of_range);

	ctx.addString("foo");
	EXPECT_EQ("foo", ctx.getString(0));
	ASSERT_THROW(ctx.getString(1), std::out_of_range);

	ctx.addString("bar");
	EXPECT_EQ("foo", ctx.getString(0));
	EXPECT_EQ("bar", ctx.getString(1));
	ASSERT_THROW(ctx.getString(2), std::out_of_range);
}

TEST(DeserializationContextTest, ObjectTraits) {
	DeserializationContext ctx;

	ASSERT_THROW(ctx.getTraits(0), std::out_of_range);
	ASSERT_THROW(ctx.getTraits(1), std::out_of_range);

	AmfObjectTraits o1("foo", true, true);
	ctx.addTraits(o1);
	EXPECT_EQ(o1, ctx.getTraits(0));
	ASSERT_THROW(ctx.getTraits(1), std::out_of_range);

	AmfObjectTraits o2("bar", false, false);
	ctx.addTraits(o2);
	EXPECT_EQ(o1, ctx.getTraits(0));
	EXPECT_EQ(o2, ctx.getTraits(1));
	ASSERT_THROW(ctx.getTraits(2), std::out_of_range);
}

TEST(DeserializationContextTest, Item) {
	DeserializationContext ctx;

	// Verify no objects are stored yet.
	ASSERT_THROW(ctx.getObject<AmfNull>(0), std::out_of_range);

	// Add one.
	ctx.addObject(AmfNull());
	ASSERT_EQ(AmfNull(), ctx.getObject<AmfNull>(0));
	ASSERT_THROW(ctx.getObject<AmfInteger>(0), std::invalid_argument);

	// Another one.
	AmfInteger i(17);
	ctx.addObject(i);
	ASSERT_EQ(AmfNull(), ctx.getObject<AmfNull>(0));
	ASSERT_EQ(i, ctx.getObject<AmfInteger>(1));

	// Verify that a copy is made.
	i.value = 21;
	ASSERT_EQ(AmfInteger(17), ctx.getObject<AmfInteger>(1));
}

TEST(DeserializationContext, Pointer) {
	DeserializationContext ctx;

	ASSERT_THROW(ctx.getPointer<AmfInteger>(0), std::out_of_range);

	ctx.addPointer(AmfItemPtr(AmfNull()));
	ASSERT_EQ(AmfNull(), ctx.getPointer<AmfNull>(0).as<AmfNull>());
	ASSERT_EQ(AmfNull(), ctx.getObject<AmfNull>(0));
	ASSERT_THROW(ctx.getPointer<AmfInteger>(0), std::invalid_argument);
	ASSERT_THROW(ctx.getObject<AmfInteger>(0), std::invalid_argument);
	ASSERT_THROW(ctx.getPointer<AmfNull>(1), std::out_of_range);
	ASSERT_THROW(ctx.getObject<AmfNull>(1), std::out_of_range);

	ctx.addObject(AmfInteger(11));
	ASSERT_EQ(AmfNull(), ctx.getPointer<AmfNull>(0).as<AmfNull>());
	ASSERT_EQ(AmfInteger(11), ctx.getPointer<AmfInteger>(1).as<AmfInteger>());
	ASSERT_EQ(AmfInteger(11), ctx.getObject<AmfInteger>(1));
	ASSERT_THROW(ctx.getPointer<AmfDouble>(1), std::invalid_argument);
	ASSERT_THROW(ctx.getObject<AmfDouble>(1), std::invalid_argument);
}

TEST(DeserializationContextTest, Clear) {
	DeserializationContext ctx;

	// Verify the context is initially empty.
	ASSERT_THROW(ctx.getString(0), std::out_of_range);
	ASSERT_THROW(ctx.getTraits(0), std::out_of_range);
	ASSERT_THROW(ctx.getObject<AmfNull>(0), std::out_of_range);

	// Add a few objects.
	ctx.addString("foo");
	ctx.addString("bar");
	ctx.addTraits(AmfObjectTraits("asd", false, false));
	ctx.addTraits(AmfObjectTraits("foobar", false, false));
	ctx.addTraits(AmfObjectTraits("qux", true, false));
	ctx.addObject(AmfInteger(17));
	ctx.addObject(AmfDouble(1.0));

	ASSERT_NO_THROW(ctx.getString(1));
	ASSERT_NO_THROW(ctx.getTraits(2));
	ASSERT_NO_THROW(ctx.getObject<AmfDouble>(1));

	// Clear and check that it's empty again.
	ctx.clear();
	ASSERT_THROW(ctx.getString(0), std::out_of_range);
	ASSERT_THROW(ctx.getTraits(0), std::out_of_range);
	ASSERT_THROW(ctx.getObject<AmfNull>(0), std::out_of_range);
}

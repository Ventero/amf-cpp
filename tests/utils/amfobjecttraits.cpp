#include "amftest.hpp"

#include "utils/amfobjecttraits.hpp"

TEST(AmfObjectTraitsTest, Construction) {
	AmfObjectTraits obj("foo", true, false);
	EXPECT_EQ("foo", obj.className);
	EXPECT_EQ(true, obj.dynamic);
	EXPECT_EQ(false, obj.externalizable);
	EXPECT_EQ(std::vector<std::string>(), obj.getAttriutes());
}

TEST(AmfObjectTraitsTest, Equality) {
	AmfObjectTraits obj1("foo", false, true);
	AmfObjectTraits obj2("foo", false, false);
	AmfObjectTraits obj3("foo", true, false);
	AmfObjectTraits obj4("foo", false, false);
	AmfObjectTraits obj5("bar", false, true);
	AmfObjectTraits obj1e("foo", false, true);

	EXPECT_NE(obj1, obj2);
	EXPECT_NE(obj1, obj3);
	EXPECT_NE(obj1, obj4);
	EXPECT_NE(obj1, obj5);
	EXPECT_EQ(obj1, obj1e);

	// Verify attributes are also compared.
	obj1.addAttribute("attr");
	EXPECT_NE(obj1, obj1e);

	obj1e.addAttribute("attr");
	EXPECT_EQ(obj1, obj1e);

	// Verify attributes can't be duplicated.
	obj1.addAttribute("attr");
	EXPECT_EQ(obj1, obj1e);
}

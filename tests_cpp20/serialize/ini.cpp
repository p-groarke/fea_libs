#include <fea/serialize/ini.hpp>
#include <gtest/gtest.h>

namespace {
constexpr std::string_view test_basics = R"(
	 global_var = 1 ; int

; comment ; comment
		[test!][test~]]]]test
 testme =	 "a 'test'\n" ; ; comment

	aaabbb[[[[🤣.bla][[[]]]][pppsdfsa
	   ; comment
	🙂	= "  '	 🔥 '"	; another comment

bad_section]]]]
unclosed = " unclosed ' string
unsaveable line

[type_tests]
a_bool = true
an_int = 42
a_float = 69.0
a_string = "potato"

		[test!][test~]]]]test
shouldbemerged = true
)";

TEST(ini, basics) {
	fea::ifini test{ test_basics };

	EXPECT_TRUE(test.contains(""));
	EXPECT_TRUE(test.contains("test!.test~"));
	EXPECT_TRUE(test.contains("🤣.bla"));
	EXPECT_TRUE(test.contains("bad_section"));
	EXPECT_TRUE(test.contains("type_tests"));
	EXPECT_FALSE(test.contains("potato"));

	EXPECT_TRUE(test.contains("", "global_var"));
	EXPECT_TRUE(test.contains("test!.test~", "testme"));
	EXPECT_TRUE(test.contains("🤣.bla", "🙂"));
	EXPECT_TRUE(test.contains("bad_section", "unclosed"));
	EXPECT_FALSE(test.contains("bad_section", "unsaveable line"));
	EXPECT_FALSE(test.contains("bad_section", "unsaveable"));
	EXPECT_FALSE(test.contains("bad_section", "line"));
	EXPECT_TRUE(test.contains("type_tests", "a_bool"));
	EXPECT_TRUE(test.contains("type_tests", "an_int"));
	EXPECT_TRUE(test.contains("type_tests", "a_float"));
	EXPECT_TRUE(test.contains("type_tests", "a_string"));
	EXPECT_TRUE(test.contains("test!.test~", "shouldbemerged"));

	// Test invalid values.
	{
		bool boolval = test["bla"]["bla"];
		EXPECT_EQ(boolval, false);

		int intval = test["bla"]["blee"];
		EXPECT_EQ(intval, 0);

		size_t sizetval = test["bla"]["blee"];
		EXPECT_EQ(sizetval, 0);

		unsigned uintval = test["bla"]["blou"];
		EXPECT_EQ(uintval, 0u);

		float floatval = test["fla"]["flee"];
		EXPECT_EQ(floatval, 0.f);

		std::string stringval = test["fla"]["flou"];
		EXPECT_EQ(stringval, "");

		// Test invalid with defaults.
		boolval = test["bla"]["bla"] | true;
		EXPECT_EQ(boolval, true);

		intval = test["bla"]["blee"] | 42;
		EXPECT_EQ(intval, 42);

		sizetval = test["bla"]["blee"] | 42u;
		EXPECT_EQ(sizetval, 42u);

		uintval = test["bla"]["blou"] | 69u;
		EXPECT_EQ(uintval, 69u);

		floatval = test["fla"]["flee"] | -42.f;
		EXPECT_EQ(floatval, -42.f);

		stringval = test["fla"]["flou"] | "a default";
		EXPECT_EQ(stringval, "a default");
	}

	// Test expected.
	{

		int global_var = test[""]["global_var"];
		EXPECT_EQ(global_var, 1);

		std::string testme = test["test!.test~"]["testme"];
		EXPECT_EQ(testme, "a 'test'\\n");

		std::string smiley = test["🤣.bla"]["🙂"];
		EXPECT_EQ(smiley, "  '	 🔥 '");

		std::string unclosed = test["bad_section"]["unclosed"];
		EXPECT_EQ(unclosed, " unclosed ' string'");

		bool a_bool = test["type_tests"]["a_bool"];
		EXPECT_EQ(a_bool, true);

		int an_int = test["type_tests"]["an_int"];
		EXPECT_EQ(an_int, 42);

		float a_float = test["type_tests"]["a_float"];
		EXPECT_EQ(a_float, 69.f);

		std::string_view a_string = test["type_tests"]["a_string"];
		EXPECT_EQ(a_string, "potato");

		bool shouldbemerged = test["test!.test~"]["shouldbemerged"];
		EXPECT_EQ(shouldbemerged, true);
	}

	//[type_tests]
	// a_bool = true
	// an_int = 42
	// a_float = 69.0
	// a_string = "potato"
	//
	//		[test!][test~]]]]test
	// shouldbemerged = true

	// EXPECT_TRUE(test.contains("🤣.bla", "🙂"));
	// EXPECT_TRUE(test.contains("bad_section", "var"));
	// EXPECT_FALSE(test.contains("bad_section", "unsaveable line"));
	// EXPECT_FALSE(test.contains("bad_section", "unsaveable"));
	// EXPECT_FALSE(test.contains("bad_section", "line"));
	// EXPECT_TRUE(test.contains("type_tests", "a_bool"));
	// EXPECT_TRUE(test.contains("type_tests", "an_int"));
	// EXPECT_TRUE(test.contains("type_tests", "a_float"));
	// EXPECT_TRUE(test.contains("type_tests", "a_string"));
	// EXPECT_TRUE(test.contains("test!.test~", "shouldbemerged"));
}
} // namespace
#include <fea/serialize/ini.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

namespace {
TEST(ini, example) {
	// LOL
	const float default_float = 69.f;
	fea::ini f{ std::filesystem::path{ "file.ini" } };
	f.general_help(false);

	// Return type overload.
	// If the variable isn't found, returns | default.
	// Assigns the default if ini object isn't const (and it doesn't contain
	// "a_float").
	[[maybe_unused]] float b = f["section"]["a_float"] | default_float;

	// Assignement.
	// Also adds a comment to the output.
	f["section"]["an_int"] = 42, "Int comment";

	// Add a section comment.
	f["section"], "Section Comment";

	// Write to "file.ini".
	f.write();

	/* Outputs :
	; Section Comment
	[section]
	a_float = 69.0
	  ; Int comment
	an_int = 42
	*/
}

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

[section with spaces]
)";

TEST(ini, basics) {
	fea::ini test{ test_basics };

	EXPECT_TRUE(test.contains(""));
	EXPECT_TRUE(test.contains("test!.test~"));
	EXPECT_TRUE(test.contains("🤣.bla"));
	EXPECT_TRUE(test.contains("bad_section"));
	EXPECT_TRUE(test.contains("type_tests"));
	EXPECT_TRUE(test.contains("section with spaces"));
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

	// Test non-existing values (defaulted).
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

#if FEA_WINDOWS
		stringval = test["fla"]["flou"] | "a default";
		EXPECT_EQ(stringval, "a default");
#else
		stringval = static_cast<std::string>(test["fla"]["flou"] | "a default");
		EXPECT_EQ(stringval, "a default");

		stringval = std::string(test["fla"]["flou"] | "a default");
		EXPECT_EQ(stringval, "a default");
#endif
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

		std::string a_string = test["type_tests"]["a_string"];
		EXPECT_EQ(a_string, "potato");

		bool shouldbemerged = test["test!.test~"]["shouldbemerged"];
		EXPECT_EQ(shouldbemerged, true);
	}

	// Writing.
	{
		test["bla"]["bla"] = true, "bla.bla\ncomment";
		bool boolval = test["bla"]["bla"];
		EXPECT_EQ(boolval, true);

		test["bla"]["blee"] = 42, "bla.blee comment";
		int intval = test["bla"]["blee"];
		EXPECT_EQ(intval, 42);

		test["bla"]["flee"] = size_t(101u), "bla.flee comment";
		size_t sizetval = test["bla"]["flee"];
		EXPECT_EQ(sizetval, 101u);

		test["bla"]["blou"] = 2u, "bla.blou comment";
		unsigned uintval = test["bla"]["blou"];
		EXPECT_EQ(uintval, 2u);

		test["fla"]["flee"] = -5.5000f, "fla.flee comment";
		float floatval = test["fla"]["flee"];
		EXPECT_EQ(floatval, -5.5f);

		test["fla"]["flou"] = "test write", "fla.flou comment";
		std::string stringval = test["fla"]["flou"];
		EXPECT_EQ(stringval, "test write");

		test["fla"]["flou2"] = std::string{ "test write2" },
		"fla.flou2 comment";
		std::string stringval2 = test["fla"]["flou2"];
		EXPECT_EQ(stringval2, "test write2");

		test["fla"]["flou3"] = std::string_view{ "test write3" },
		"fla.flou3 comment";
		std::string stringval3 = test["fla"]["flou3"];
		EXPECT_EQ(stringval3, "test write3");
	}

	EXPECT_TRUE(test.contains("bla", "bla"));
	EXPECT_TRUE(test.contains("bla", "blee"));
	EXPECT_TRUE(test.contains("bla", "flee"));
	EXPECT_TRUE(test.contains("bla", "blou"));
	EXPECT_TRUE(test.contains("fla", "flee"));
	EXPECT_TRUE(test.contains("fla", "flou"));
	EXPECT_TRUE(test.contains("fla", "flou2"));
	EXPECT_TRUE(test.contains("fla", "flou3"));

	// Defaults after writing.
	{
		bool boolval = test["bla"]["bla"] | false;
		EXPECT_EQ(boolval, true);

		int intval = test["bla"]["blee"] | -1;
		EXPECT_EQ(intval, 42);

		size_t sizetval = test["bla"]["flee"] | size_t(0u);
		EXPECT_EQ(sizetval, 101u);

		unsigned uintval = test["bla"]["blou"] | 42u;
		EXPECT_EQ(uintval, 2u);

		float floatval = test["fla"]["flee"] | -1.f;
		EXPECT_EQ(floatval, -5.5f);

		std::string stringval = test["fla"]["flou"] | "default";
		EXPECT_EQ(stringval, "test write");

		std::string stringval2
				= test["fla"]["flou2"] | std::string{ "default" };
		EXPECT_EQ(stringval2, "test write2");

		std::string stringval3
				= test["fla"]["flou3"] | std::string_view{ "default" };
		EXPECT_EQ(stringval3, "test write3");
	}

	// Invalid types.
	{
		// Wrong return type. Should cast to return type.
		// No internal type change.
		int boolval = test["bla"]["bla"];
		EXPECT_EQ(boolval, int(true));

		// Wrong return type and default value.
		// Since we already contain the value :
		// It's internal type will be changed to float.
		// It will be cast to the return capture int.
		boolval = test["bla"]["bla"] | float(0);
		EXPECT_EQ(boolval, int(true));

		// Return value cast, no internal type change.
		float intval = test["bla"]["blee"];
		EXPECT_EQ(intval, 42.f);

		// Transforms internally to string, ignores default value, casts return
		// value to float.
		intval = test["bla"]["blee"] | "42.5";
		EXPECT_EQ(intval, 42.f);

		// Transforms internally to string, converts return value to float.
		// Default ignored.
		float floatval = test["fla"]["flee"] | "42.0";
		EXPECT_EQ(floatval, -5.5f);

		// Transforms internally to int, cast return to float.
		floatval = test["fla"]["flee"] | int(0);
		EXPECT_EQ(floatval, -5.f);
	}

	// Comments and output.
	{
		test["bla"], "bla comment";
		test["fla"], "fla comment";

		std::string got = fea::to_string(test);
		EXPECT_NE(got.find("; bla comment"), got.npos);
		EXPECT_NE(got.find("; fla comment"), got.npos);
		EXPECT_NE(got.find("; bla.bla\n  ; comment"), got.npos);
		EXPECT_NE(got.find("; bla.blee comment"), got.npos);
		EXPECT_NE(got.find("; bla.flee comment"), got.npos);
		EXPECT_NE(got.find("; fla.flou comment"), got.npos);
		EXPECT_NE(got.find("; fla.flou2 comment"), got.npos);
		EXPECT_NE(got.find("; fla.flou3 comment"), got.npos);

		test.general_help(true);
		got = fea::to_string(test);
		EXPECT_NE(got.find(fea::detail::ini::general_help), got.npos);

		test.general_help(false);
		got = fea::to_string(test);
		EXPECT_EQ(got.find(fea::detail::ini::general_help), got.npos);

		// Update if modifying variable help string.
		test.variable_help(false);
		got = fea::to_string(test);
		EXPECT_EQ(got.find("Expects a "), got.npos);

		// Update if modifying variable help string.
		test.variable_help(true);
		got = fea::to_string(test);
		EXPECT_NE(got.find("Expects a "), got.npos);


		//  std::cout << fea::to_string(test) << std::endl;
		test.write("test_output.ini");
	}
}
} // namespace
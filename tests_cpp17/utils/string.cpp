#include <fea/utils/string.hpp>
#include <gtest/gtest.h>

namespace {
TEST(string, basics) {
	std::string str = "a string weeee, bang, ding, ow";
	EXPECT_TRUE(fea::contains(str, "ding"));
	EXPECT_FALSE(fea::contains(str, "dong"));

	std::string caps = "NOT SCREAMING";
	EXPECT_EQ(fea::to_lower(caps), "not screaming");
	EXPECT_NE(fea::to_lower(caps), "NOT SCREAMING");

	std::string capscpy = caps;
	fea::to_lower(capscpy, true);
	EXPECT_EQ(capscpy, "not screaming");
	EXPECT_NE(capscpy, "NOT SCREAMING");

	auto vec = fea::split(str, ',');
	std::vector<std::string> vec_answer{ "a string weeee", " bang", " ding",
		" ow" };
	EXPECT_EQ(vec.size(), 4u);
	EXPECT_EQ(vec, vec_answer);

	vec = fea::split(str, ", ");
	vec_answer = { "a", "string", "weeee", "bang", "ding", "ow" };
	EXPECT_EQ(vec.size(), 6u);
	EXPECT_EQ(vec, vec_answer);

	EXPECT_EQ(fea::replace_all(caps, "NOT", "is"), "is SCREAMING");

	capscpy = caps;
	fea::replace_all(capscpy, "NOT", "is", true);
	EXPECT_EQ(capscpy, "is SCREAMING");

	fea::replace_all(capscpy, "is", "is not", true);
	EXPECT_EQ(capscpy, "is not SCREAMING");

	fea::replace_all(capscpy, "is", "is", true);
	EXPECT_EQ(capscpy, "is not SCREAMING");
}

#if FEA_CPP20
struct str {
	str(const char* s)
			: data(s) {
	}
	str() = default;

	friend std::strong_ordering operator<=>(const str& lhs, const str& rhs) {
		return fea::lexicographical_compare(lhs.data.begin(), lhs.data.end(),
				rhs.data.begin(), rhs.data.end());
	}
	friend bool operator==(const str& lhs, const str& rhs) {
		// Don't use operator<=>
		return lhs.data == rhs.data;
	}

	std::string data;
};

TEST(string, lexicographical_compare) {

	std::vector<str> vec{ "abc", "abcd", "Abc", "aBc", "ABC", "ABCD", "Bob",
		"bobby", "0", "1", "10", "2", "22", "As", "Aster", "Astrolabe",
		"Astronomy", "astrophysics", "At", "Ataman", "Attack", "Baa",
		"Barnacle", "Be", "been", "Benefit", "Bent" };

	const std::vector<str> answer{ "0", "1", "10", "2", "22", "abc", "Abc",
		"aBc", "ABC", "abcd", "ABCD", "As", "Aster", "Astrolabe", "Astronomy",
		"astrophysics", "At", "Ataman", "Attack", "Baa", "Barnacle", "Be",
		"been", "Benefit", "Bent", "Bob", "bobby" };

	std::sort(vec.begin(), vec.end());
	// for (const auto& str : vec) {
	//	printf("%s\n", str.data.c_str());
	//}
	EXPECT_EQ(answer, vec);
}
#endif
} // namespace

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
}
} // namespace

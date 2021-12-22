#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/string/string.hpp>
#include <gtest/gtest.h>
#include <tuple>

namespace {
#define gen_constants(tupname, str) \
	const auto tupname = std::tuple{ \
		std::string_view{ str }, \
		std::wstring_view{ L##str }, \
		std::u16string_view{ u##str }, \
		std::u32string_view{ U##str }, \
		std::string{ str }, \
		std::wstring{ L##str }, \
		std::u16string{ u##str }, \
		std::u32string{ U##str }, \
		str, \
		L##str, \
		u##str, \
		U##str, \
	};


#define gen_tests(tupname, ...) \
	const auto tupname = std::tuple{ \
		std::array{ FEA_VA_SV(__VA_ARGS__) }, \
		std::array{ FEA_VA_WSV(__VA_ARGS__) }, \
		std::array{ FEA_VA_U16SV(__VA_ARGS__) }, \
		std::array{ FEA_VA_U32SV(__VA_ARGS__) }, \
		std::array{ FEA_VA_S(__VA_ARGS__) }, \
		std::array{ FEA_VA_WS(__VA_ARGS__) }, \
		std::array{ FEA_VA_U16S(__VA_ARGS__) }, \
		std::array{ FEA_VA_U32S(__VA_ARGS__) }, \
		std::array{ __VA_ARGS__ }, \
		std::array{ FEA_VA_WPREFIX(__VA_ARGS__) }, \
		std::array{ FEA_VA_U16PREFIX(__VA_ARGS__) }, \
		std::array{ FEA_VA_U32PREFIX(__VA_ARGS__) }, \
	};


TEST(string, basics) {
	{
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

		auto vec = fea::split(str, ",");
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
		fea::replace_all_inplace(capscpy, "NOT", "is");
		EXPECT_EQ(capscpy, "is SCREAMING");

		fea::replace_all_inplace(capscpy, "is", "is not");
		EXPECT_EQ(capscpy, "is not SCREAMING");

		fea::replace_all_inplace(capscpy, "is", "is");
		EXPECT_EQ(capscpy, "is not SCREAMING");
	}
	{
		const std::string test_str = "test";
		const std::wstring wtest_str = L"test";

		EXPECT_FALSE(fea::starts_with(test_str, "test1"));
		EXPECT_FALSE(fea::starts_with(test_str, "1test"));
		EXPECT_TRUE(fea::starts_with(test_str, "\0"));
		EXPECT_TRUE(fea::starts_with(test_str, "test"));
		EXPECT_TRUE(fea::starts_with(test_str, "tes"));
		EXPECT_TRUE(fea::starts_with(test_str, "t"));
		EXPECT_FALSE(fea::starts_with(test_str, std::string{ "test1" }));
		EXPECT_FALSE(fea::starts_with(test_str, std::string{ "1test" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "\0" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "test" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "tes" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "t" }));

		EXPECT_FALSE(fea::starts_with(wtest_str, L"test1"));
		EXPECT_FALSE(fea::starts_with(wtest_str, L"1test"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"\0"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"test"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"tes"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"t"));
		EXPECT_FALSE(fea::starts_with(wtest_str, std::wstring{ L"test1" }));
		EXPECT_FALSE(fea::starts_with(wtest_str, std::wstring{ L"1test" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"\0" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"test" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"tes" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"t" }));

		EXPECT_FALSE(fea::ends_with(test_str, "test1"));
		EXPECT_FALSE(fea::ends_with(test_str, "1test"));
		EXPECT_TRUE(fea::ends_with(test_str, "\0"));
		EXPECT_TRUE(fea::ends_with(test_str, "test"));
		EXPECT_TRUE(fea::ends_with(test_str, "est"));
		EXPECT_TRUE(fea::ends_with(test_str, "t"));
		EXPECT_FALSE(fea::ends_with(test_str, std::string{ "test1" }));
		EXPECT_FALSE(fea::ends_with(test_str, std::string{ "1test" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "\0" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "test" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "est" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "t" }));

		EXPECT_FALSE(fea::ends_with(wtest_str, L"test1"));
		EXPECT_FALSE(fea::ends_with(wtest_str, L"1test"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"\0"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"test"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"est"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"t"));
		EXPECT_FALSE(fea::ends_with(wtest_str, std::wstring{ L"test1" }));
		EXPECT_FALSE(fea::ends_with(wtest_str, std::wstring{ L"1test" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"\0" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"test" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"est" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"t" }));
	}
}

gen_constants(alice, R"xx(CHAPTER I.
Down the Rabbit-Hole

Alice was beginning to get very tired of sitting by her sister on the bank, and
of having nothing to do: once or twice she had peeped into the book her sister
was reading, but it had no pictures or conversations in it, “and what is the
use of a book,” thought Alice “without pictures or conversations?”

So she was considering in her own mind (as well as she could, for the hot day
made her feel very sleepy and stupid), whether the pleasure of making a
daisy-chain would be worth the trouble of getting up and picking the daisies,
when suddenly a White Rabbit with pink eyes ran close by her.

There was nothing so very remarkable in that; nor did Alice think it so very
much out of the way to hear the Rabbit say to itself, “Oh dear! Oh dear! I
shall be late!” (when she thought it over afterwards, it occurred to her that
she ought to have wondered at this, but at the time it all seemed quite
natural); but when the Rabbit actually took a watch out of its
waistcoat-pocket, and looked at it, and then hurried on, Alice started to her
feet, for it flashed across her mind that she had never before seen a rabbit
with either a waistcoat-pocket, or a watch to take out of it, and burning with
curiosity, she ran across the field after it, and fortunately was just in time
to see it pop down a large rabbit-hole under the hedge.
)xx");

TEST(string, contains) {
	gen_tests(valid_search, "I.", "”", "Alice", "Rabbit-Hole",
			"under the hedge.", "\n");

	gen_tests(invalid_search, ">test", ">", "1potato", "/I'm a sentence");

	constexpr size_t num_str_types = std::tuple_size_v<decltype(alice)>;

	fea::static_for<num_str_types>([&](auto const_i) {
		constexpr size_t i = const_i;

		const auto& test_str = std::get<i>(alice);
		using str_t = std::decay_t<decltype(test_str)>;

		const auto& valid_arr = std::get<i>(valid_search);
		for (const auto& search : valid_arr) {
			EXPECT_TRUE(fea::contains(test_str, search));
			EXPECT_TRUE(fea::contains(test_str, search[0]));
			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_TRUE(fea::contains(test_str, search.data()));
			}
		}

		const auto& invalid_arr = std::get<i>(invalid_search);
		for (const auto& search : invalid_arr) {
			EXPECT_FALSE(fea::contains(test_str, search));
			EXPECT_FALSE(fea::contains(test_str, search[0]));
			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_FALSE(fea::contains(test_str, search.data()));
			}
		}
	});
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

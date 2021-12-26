#include <array>
#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/string/replace.hpp>
#include <fea/string/string.hpp>
#include <gtest/gtest.h>
#include <tuple>

#define gen_constants(tupname, str) \
	const auto tupname = std::make_tuple(std::string_view{ str }, \
			std::wstring_view{ L##str }, std::u16string_view{ u##str }, \
			std::u32string_view{ U##str }, std::string{ str }, \
			std::wstring{ L##str }, std::u16string{ u##str }, \
			std::u32string{ U##str }, str, L##str, u##str, U##str)


#define gen_tests(tupname, ...) \
	const auto tupname = std::make_tuple(std::array{ FEA_VA_SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_WSV(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_S(__VA_ARGS__) }, \
			std::array{ FEA_VA_WS(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16S(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32S(__VA_ARGS__) }, std::array{ __VA_ARGS__ }, \
			std::array{ FEA_VA_WPREFIX(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16PREFIX(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32PREFIX(__VA_ARGS__) })


namespace {
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

gen_constants(alice_answer, R"xx(CHAPTER I.
Down potato Rabbit-Hole

Alice was beginning to get very tired of sitting by her sister on potato bank, and
of having nothing to do: once or twice she had peeped into potato book her sister
was reading, but it had no pictures or conversations in it, “and what is potato
use of a book,” thought Alice “without pictures or conversations?”

So she was considering in her own mind (as well as she could, for potato hot day
made her feel very sleepy and stupid), whepotator potato pleasure of making a
daisy-chain would be worth potato trouble of getting up and picking potato daisies,
when suddenly a White Rabbit with pink eyes ran close by her.

There was nothing so very remarkable in that; nor did Alice think it so very
much out of potato way to hear potato Rabbit say to itself, “Oh dear! Oh dear! I
shall be late!” (when she thought it over afterwards, it occurred to her that
she ought to have wondered at this, but at potato time it all seemed quite
natural); but when potato Rabbit actually took a watch out of its
waistcoat-pocket, and looked at it, and potaton hurried on, Alice started to her
feet, for it flashed across her mind that she had never before seen a rabbit
with eipotator a waistcoat-pocket, or a watch to take out of it, and burning with
curiosity, she ran across potato field after it, and fortunately was just in time
to see it pop down a large rabbit-hole under potato hedge.
)xx");

TEST(string, replace_all) {
	gen_tests(the_test, "the", "potato");

	constexpr size_t s = std::tuple_size_v<decltype(alice)>;
	fea::static_for<s>([&](auto const_i) {
		constexpr size_t i = const_i;
		const auto& str = std::get<i>(alice);
		const auto& answer = std::get<i>(alice_answer);
		const auto& search = std::get<i>(the_test)[0];
		const auto& replace = std::get<i>(the_test)[1];

		auto new_str = fea::replace_all(str, search, replace);
		EXPECT_EQ(answer, new_str);
	});
}
} // namespace

#include <fea/enum/enum_to_strings.hpp>
#include <gtest/gtest.h>

namespace {
FEA_STRING_ENUM(my_enum, unsigned, potato, tomato)

TEST(enum_to_strings, to_string) {
	using namespace std::literals::string_view_literals;

	EXPECT_EQ(to_string(my_enum::potato), "potato"sv);
	EXPECT_EQ(to_string(my_enum::tomato), "tomato"sv);
	EXPECT_EQ(to_string(my_enum::potato), L"potato"sv);
	EXPECT_EQ(to_string(my_enum::tomato), L"tomato"sv);
	EXPECT_EQ(to_string(my_enum::potato), u"potato"sv);
	EXPECT_EQ(to_string(my_enum::tomato), u"tomato"sv);
	EXPECT_EQ(to_string(my_enum::potato), U"potato"sv);
	EXPECT_EQ(to_string(my_enum::tomato), U"tomato"sv);
	EXPECT_EQ(to_string(my_enum::potato), u8"potato"sv);
	EXPECT_EQ(to_string(my_enum::tomato), u8"tomato"sv);

	std::string_view svp = to_string(my_enum::potato);
	std::string_view svt = to_string(my_enum::tomato);
	std::wstring_view wsvp = to_string(my_enum::potato);
	std::wstring_view wsvt = to_string(my_enum::tomato);
	std::u16string_view u16svp = to_string(my_enum::potato);
	std::u16string_view u16svt = to_string(my_enum::tomato);
	std::u32string_view u32svp = to_string(my_enum::potato);
	std::u32string_view u32svt = to_string(my_enum::tomato);
	std::u8string_view u8svp = to_string(my_enum::potato);
	std::u8string_view u8svt = to_string(my_enum::tomato);
	EXPECT_EQ(svp, "potato"sv);
	EXPECT_EQ(svt, "tomato"sv);
	EXPECT_EQ(wsvp, L"potato"sv);
	EXPECT_EQ(wsvt, L"tomato"sv);
	EXPECT_EQ(u16svp, u"potato"sv);
	EXPECT_EQ(u16svt, u"tomato"sv);
	EXPECT_EQ(u32svp, U"potato"sv);
	EXPECT_EQ(u32svt, U"tomato"sv);
	EXPECT_EQ(u8svp, u8"potato"sv);
	EXPECT_EQ(u8svt, u8"tomato"sv);

	std::string strp = to_string(my_enum::potato);
	std::string strt = to_string(my_enum::tomato);
	std::wstring wstrp = to_string(my_enum::potato);
	std::wstring wstrt = to_string(my_enum::tomato);
	std::u16string u16strp = to_string(my_enum::potato);
	std::u16string u16strt = to_string(my_enum::tomato);
	std::u32string u32strp = to_string(my_enum::potato);
	std::u32string u32strt = to_string(my_enum::tomato);
	std::u8string u8strp = to_string(my_enum::potato);
	std::u8string u8strt = to_string(my_enum::tomato);

	EXPECT_EQ(strp, "potato");
	EXPECT_EQ(strt, "tomato");
	EXPECT_EQ(wstrp, L"potato");
	EXPECT_EQ(wstrt, L"tomato");
	EXPECT_EQ(u16strp, u"potato");
	EXPECT_EQ(u16strt, u"tomato");
	EXPECT_EQ(u32strp, U"potato");
	EXPECT_EQ(u32strt, U"tomato");
	EXPECT_EQ(u8strp, u8"potato");
	EXPECT_EQ(u8strt, u8"tomato");
}

TEST(enum_to_strings, from_string) {
	using namespace std::literals::string_view_literals;

	my_enum e;
	EXPECT_TRUE(from_string("potato"sv, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string("tomato"sv, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(L"potato"sv, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(L"tomato"sv, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(u"potato"sv, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(u"tomato"sv, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(U"potato"sv, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(U"tomato"sv, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(u8"potato"sv, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(u8"tomato"sv, e));
	EXPECT_EQ(e, my_enum::tomato);

	EXPECT_TRUE(from_string(std::string{ "potato" }, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(std::string{ "tomato" }, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(std::wstring{ L"potato" }, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(std::wstring{ L"tomato" }, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(std::u16string{ u"potato" }, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(std::u16string{ u"tomato" }, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(std::u32string{ U"potato" }, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(std::u32string{ U"tomato" }, e));
	EXPECT_EQ(e, my_enum::tomato);
	EXPECT_TRUE(from_string(std::u8string{ u8"potato" }, e));
	EXPECT_EQ(e, my_enum::potato);
	EXPECT_TRUE(from_string(std::u8string{ u8"tomato" }, e));
	EXPECT_EQ(e, my_enum::tomato);


	EXPECT_FALSE(from_string("test"sv, e));
	EXPECT_FALSE(from_string("invalid"sv, e));
	EXPECT_FALSE(from_string("count"sv, e));
	EXPECT_FALSE(from_string("Potato"sv, e));
	EXPECT_FALSE(from_string("Totato"sv, e));
}
} // namespace

#include <fea/enum/enum_to_strings.hpp>

#if FEA_GCC_GE(13)
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

	std::string_view svp = to_string(my_enum::potato);
	std::string_view svt = to_string(my_enum::tomato);
	std::wstring_view wsvp = to_string(my_enum::potato);
	std::wstring_view wsvt = to_string(my_enum::tomato);
	std::u16string_view u16svp = to_string(my_enum::potato);
	std::u16string_view u16svt = to_string(my_enum::tomato);
	std::u32string_view u32svp = to_string(my_enum::potato);
	std::u32string_view u32svt = to_string(my_enum::tomato);
	EXPECT_EQ(svp, "potato"sv);
	EXPECT_EQ(svt, "tomato"sv);
	EXPECT_EQ(wsvp, L"potato"sv);
	EXPECT_EQ(wsvt, L"tomato"sv);
	EXPECT_EQ(u16svp, u"potato"sv);
	EXPECT_EQ(u16svt, u"tomato"sv);
	EXPECT_EQ(u32svp, U"potato"sv);
	EXPECT_EQ(u32svt, U"tomato"sv);

	std::string strp = to_string(my_enum::potato);
	std::string strt = to_string(my_enum::tomato);
	std::wstring wstrp = to_string(my_enum::potato);
	std::wstring wstrt = to_string(my_enum::tomato);
	std::u16string u16strp = to_string(my_enum::potato);
	std::u16string u16strt = to_string(my_enum::tomato);
	std::u32string u32strp = to_string(my_enum::potato);
	std::u32string u32strt = to_string(my_enum::tomato);

	EXPECT_EQ(strp, "potato");
	EXPECT_EQ(strt, "tomato");
	EXPECT_EQ(wstrp, L"potato");
	EXPECT_EQ(wstrt, L"tomato");
	EXPECT_EQ(u16strp, u"potato");
	EXPECT_EQ(u16strt, u"tomato");
	EXPECT_EQ(u32strp, U"potato");
	EXPECT_EQ(u32strt, U"tomato");
}

TEST(enum_to_strings, from_string) {
	using namespace std::literals::string_view_literals;

	EXPECT_EQ(from_string("potato"sv), my_enum::potato);
	EXPECT_EQ(from_string("tomato"sv), my_enum::tomato);
	EXPECT_EQ(from_string(L"potato"sv), my_enum::potato);
	EXPECT_EQ(from_string(L"tomato"sv), my_enum::tomato);
	EXPECT_EQ(from_string(u"potato"sv), my_enum::potato);
	EXPECT_EQ(from_string(u"tomato"sv), my_enum::tomato);
	EXPECT_EQ(from_string(U"potato"sv), my_enum::potato);
	EXPECT_EQ(from_string(U"tomato"sv), my_enum::tomato);

	EXPECT_EQ(from_string(std::string{ "potato" }), my_enum::potato);
	EXPECT_EQ(from_string(std::string{ "tomato" }), my_enum::tomato);
	EXPECT_EQ(from_string(std::wstring{ L"potato" }), my_enum::potato);
	EXPECT_EQ(from_string(std::wstring{ L"tomato" }), my_enum::tomato);
	EXPECT_EQ(from_string(std::u16string{ u"potato" }), my_enum::potato);
	EXPECT_EQ(from_string(std::u16string{ u"tomato" }), my_enum::tomato);
	EXPECT_EQ(from_string(std::u32string{ U"potato" }), my_enum::potato);
	EXPECT_EQ(from_string(std::u32string{ U"tomato" }), my_enum::tomato);
}
} // namespace
#endif

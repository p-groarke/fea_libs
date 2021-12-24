#include <array>
#include <fea/macros/literals.hpp>
#include <fea/string/conversions.hpp>
#include <gtest/gtest.h>

namespace {
TEST(macros_literals, prefix) {
	// If this compiles the literals are well formed.
	std::string t1{ FEA_PREFIX("test") };
	std::wstring t2{ FEA_WPREFIX("test") };
	std::u16string t3{ FEA_U16PREFIX("test") };
	std::u32string t4{ FEA_U32PREFIX("test") };

	EXPECT_EQ("test", t1);
	EXPECT_EQ(L"test", t2);
	EXPECT_EQ(u"test", t3);
	EXPECT_EQ(U"test", t4);

	std::array<const char*, 4> arr1{ FEA_VA_PREFIX("0", "1", "2", "3") };
	std::array<const wchar_t*, 4> arr2{ FEA_VA_WPREFIX("0", "1", "2", "3") };
	std::array<const char16_t*, 4> arr3{ FEA_VA_U16PREFIX("0", "1", "2", "3") };
	std::array<const char32_t*, 4> arr4{ FEA_VA_U32PREFIX("0", "1", "2", "3") };

	for (size_t i = 0; i < arr1.size(); ++i) {
		EXPECT_EQ(std::to_string(i), arr1[i]);
		EXPECT_EQ(std::to_wstring(i), arr2[i]);
		EXPECT_EQ(fea::utf8_to_any<char16_t>(std::to_string(i)), arr3[i]);
		EXPECT_EQ(fea::any_to_utf32(std::to_string(i)), arr4[i]);
	}
}
} // namespace

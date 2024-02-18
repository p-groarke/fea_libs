#include <fea/terminal/pipe.hpp>
#include <gtest/gtest.h>
#include <string>

namespace {
constexpr std::string_view expected = "l1 🙂\nl2\n<>\né\n";
constexpr std::wstring_view wexpected = L"l1 🙂\nl2\n<>\né\n";

TEST(pipe, basics) {
	{
		std::string str = fea::read_pipe_text(false);
		EXPECT_EQ(str, expected);
	}

	{
		std::wstring str = fea::wread_pipe_text(false);
		EXPECT_EQ(str, wexpected);
	}
}
} // namespace

#include <fea/terminal/pipe.hpp>
#include <gtest/gtest.h>
#include <string>

extern int test_num;

namespace {
const std::string expected = "l1 🙂\nl2\n<>\né\n";
const std::wstring wexpected = L"l1 🙂\nl2\n<>\né\n";

TEST(pipe, basics) {
	switch (test_num) {
	case 0: {
		std::string str = fea::read_pipe_text();
		EXPECT_EQ(str, expected);
	} break;
	case 1: {
		std::wstring str = fea::wread_pipe_text();
		EXPECT_EQ(str, wexpected);
	} break;
	default: {
		assert(false);
	} break;
	}
}
} // namespace

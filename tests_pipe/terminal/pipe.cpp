#include <chrono>
#include <fea/terminal/pipe.hpp>
#include <gtest/gtest.h>
#include <string>
#include <thread>

extern int test_num;

namespace {
const std::string expected = "l1 🙂\nl2\n<>\né\n";
const std::wstring wexpected = L"l1 🙂\nl2\n<>\né\n";

TEST(pipe, basics) {
	// Are we too quick?
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	switch (test_num) {
	case 0: {
		std::string str = fea::read_pipe_text();
		EXPECT_EQ(str, expected);
	} break;
	case 1: {
		std::wstring str = fea::wread_pipe_text();
		EXPECT_EQ(str, wexpected);
	} break;
	case 2: {
		// make sure we are non-blocking
		std::string str = fea::read_pipe_text();
		EXPECT_EQ(str, std::string{});
	} break;
	default: {
		assert(false);
	} break;
	}
}
} // namespace

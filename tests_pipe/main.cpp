﻿#include <fea/terminal/utf8.hpp>
#include <gtest/gtest.h>
#include <iostream>

const char* argv0;

int main(int argc, char** argv) {
	std::cin.sync_with_stdio(false);
	std::cout.sync_with_stdio(false);
	std::cerr.sync_with_stdio(false);
	std::wcin.sync_with_stdio(false);
	std::wcout.sync_with_stdio(false);
	std::wcerr.sync_with_stdio(false);

	[[maybe_unused]] auto e = fea::utf8_terminal();

	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
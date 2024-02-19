﻿#include <fea/terminal/utf8.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <iostream>

const char* argv0;
int test_num = 0;

int main(int argc, char** argv) {
	std::cin.sync_with_stdio(false);
	std::cout.sync_with_stdio(false);
	std::cerr.sync_with_stdio(false);
	std::clog.sync_with_stdio(false);
	std::wcin.sync_with_stdio(false);
	std::wcout.sync_with_stdio(false);
	std::wcerr.sync_with_stdio(false);
	std::wclog.sync_with_stdio(false);

	if (argc > 1) {
		test_num = std::stoi(argv[1]);
	}

	auto e = fea::utf8_terminal();
	fea::unused(e);

	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

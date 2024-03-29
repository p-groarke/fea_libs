﻿#include <fea/terminal/utf8.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

const char* argv0;


int main(int argc, char** argv) {
	// Just test this here so we output utf and it compiles fine on other OSes.
	auto e = fea::utf8_terminal();
	fea::unused(e);

	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

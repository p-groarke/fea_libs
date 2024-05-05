#include <fea/terminal/pipe.hpp>
#include <fea/terminal/utf8_io.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <iostream>

const char* argv0;
int test_num = 0;

int main(int argc, char** argv) {
	fea::fast_iostreams();

	if (argc > 1) {
		test_num = std::stoi(argv[1]);
	}

	auto e = fea::utf8_io();
	fea::unused(e);

	argv0 = argv[0];

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

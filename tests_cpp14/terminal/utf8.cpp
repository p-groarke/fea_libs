#include <cassert>
#include <cstdio>
#include <fea/terminal/utf8.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <iostream>

namespace {
#define MEXPECT_EQ(u, v) \
	if (u != v) \
		throw std::runtime_error { \
			"Expected u == v" \
		}

#define MEXPECT_NE(u, v) \
	if (u == v) \
		throw std::runtime_error { \
			"Expected u != v" \
		}

TEST(utf8, translate_io) {
#if FEA_WINDOWS
	int prev_in_mode = _setmode(_fileno(stdin), _O_TEXT);
	int prev_out_mode = _setmode(_fileno(stdout), _O_TEXT);
	int prev_err_mode = _setmode(_fileno(stderr), _O_TEXT);
#endif

	{
		fea::translation_resetter r
				= fea::translate_io(fea::translation_mode::binary);

#if FEA_WINDOWS
		int new_in_mode = _setmode(_fileno(stdin), _O_WTEXT);
		int new_out_mode = _setmode(_fileno(stdout), _O_WTEXT);
		int new_err_mode = _setmode(_fileno(stderr), _O_WTEXT);

		MEXPECT_NE(new_in_mode, prev_in_mode);
		MEXPECT_NE(new_out_mode, prev_out_mode);
		MEXPECT_NE(new_err_mode, prev_err_mode);
		MEXPECT_EQ(new_in_mode, _O_BINARY);
		MEXPECT_EQ(new_out_mode, _O_BINARY);
		MEXPECT_EQ(new_err_mode, _O_BINARY);
#endif
	}

#if FEA_WINDOWS
	int new_in_mode = _setmode(_fileno(stdin), _O_TEXT);
	int new_out_mode = _setmode(_fileno(stdout), _O_TEXT);
	int new_err_mode = _setmode(_fileno(stderr), _O_TEXT);

	// Should be reset.
	MEXPECT_EQ(new_in_mode, prev_in_mode);
	MEXPECT_EQ(new_out_mode, prev_out_mode);
	MEXPECT_EQ(new_err_mode, prev_err_mode);
#endif

	{
		fea::translation_resetter r = fea::translate_io(
				fea::translation_mode::binary, fea::translation_mode::ignore,
				fea::translation_mode::ignore);

#if FEA_WINDOWS
		new_in_mode = _setmode(_fileno(stdin), _O_WTEXT);
		new_out_mode = _setmode(_fileno(stdout), _O_WTEXT);
		new_err_mode = _setmode(_fileno(stderr), _O_WTEXT);

		MEXPECT_NE(new_in_mode, prev_in_mode);
		MEXPECT_EQ(new_in_mode, _O_BINARY);
		MEXPECT_EQ(new_out_mode, _O_TEXT);
		MEXPECT_EQ(new_err_mode, _O_TEXT);
#endif
	}

#if FEA_WINDOWS
	new_in_mode = _setmode(_fileno(stdin), _O_TEXT);
	new_out_mode = _setmode(_fileno(stdout), _O_TEXT);
	new_err_mode = _setmode(_fileno(stderr), _O_TEXT);

	// Should be reset.
	MEXPECT_EQ(new_in_mode, prev_in_mode);
	MEXPECT_NE(new_out_mode, prev_out_mode);
	MEXPECT_NE(new_err_mode, prev_err_mode);

	MEXPECT_EQ(new_in_mode, _O_TEXT);
	MEXPECT_EQ(new_out_mode, _O_WTEXT);
	MEXPECT_EQ(new_err_mode, _O_WTEXT);
#endif

	// Reset
#if FEA_WINDOWS
	_setmode(_fileno(stdin), _O_TEXT);
	_setmode(_fileno(stdout), _O_TEXT);
	_setmode(_fileno(stderr), _O_TEXT);
#endif
}

TEST(utf8, utf8_terminal) {
#if FEA_WINDOWS
	EXPECT_EQ(int(fea::translation_mode::text), _O_TEXT);
	EXPECT_EQ(int(fea::translation_mode::binary), _O_BINARY);
	EXPECT_EQ(int(fea::translation_mode::wtext), _O_WTEXT);
	EXPECT_EQ(int(fea::translation_mode::u16text), _O_U16TEXT);
	EXPECT_EQ(int(fea::translation_mode::u8text), _O_U8TEXT);

	SetConsoleCP(28591);
	SetConsoleOutputCP(28591);

	unsigned prev_in_cp = GetConsoleCP();
	unsigned prev_out_cp = GetConsoleOutputCP();

	int prev_in_mode = _setmode(_fileno(stdin), _O_TEXT);
	int prev_out_mode = _setmode(_fileno(stdout), _O_TEXT);
	int prev_err_mode = _setmode(_fileno(stderr), _O_TEXT);
#endif

	{
		auto e = fea::utf8_terminal();
		fea::unused(e);
		printf("Shouldn't assert.\n");

#if FEA_WINDOWS
		EXPECT_NE(GetConsoleCP(), prev_in_cp);
		EXPECT_NE(GetConsoleOutputCP(), prev_out_cp);

		// Shouldn't have changed.
		int new_in_mode = _setmode(_fileno(stdin), _O_TEXT);
		int new_out_mode = _setmode(_fileno(stdout), _O_TEXT);
		int new_err_mode = _setmode(_fileno(stderr), _O_TEXT);

		EXPECT_EQ(new_in_mode, prev_in_mode);
		EXPECT_EQ(new_out_mode, prev_out_mode);
		EXPECT_EQ(new_err_mode, prev_err_mode);
		EXPECT_EQ(new_in_mode, _O_TEXT);
		EXPECT_EQ(new_out_mode, _O_TEXT);
		EXPECT_EQ(new_err_mode, _O_TEXT);
#endif
	}

#if FEA_WINDOWS
	EXPECT_EQ(GetConsoleCP(), prev_in_cp);
	EXPECT_EQ(GetConsoleOutputCP(), prev_out_cp);

	// Shouldn't have been reset.
	int new_in_mode = _setmode(_fileno(stdin), _O_TEXT);
	int new_out_mode = _setmode(_fileno(stdout), _O_TEXT);
	int new_err_mode = _setmode(_fileno(stderr), _O_TEXT);
	EXPECT_EQ(new_in_mode, prev_in_mode);
	EXPECT_EQ(new_out_mode, prev_out_mode);
	EXPECT_EQ(new_err_mode, prev_err_mode);
	EXPECT_EQ(new_in_mode, _O_TEXT);
	EXPECT_EQ(new_out_mode, _O_TEXT);
	EXPECT_EQ(new_err_mode, _O_TEXT);
#endif

	{
		auto e = fea::utf8_terminal(true);
		fea::unused(e);

#if FEA_WINDOWS
#if FEA_DEBUG
		EXPECT_DEATH(printf("Should assert on windows.\n"), "");
#endif

		// Can't use gtest, since it outputs to cout.
		MEXPECT_NE(GetConsoleCP(), prev_in_cp);
		MEXPECT_NE(GetConsoleOutputCP(), prev_out_cp);

		new_in_mode = _setmode(_fileno(stdin), _O_BINARY);
		new_out_mode = _setmode(_fileno(stdout), _O_BINARY);
		new_err_mode = _setmode(_fileno(stderr), _O_BINARY);

		MEXPECT_NE(new_in_mode, prev_in_mode);
		MEXPECT_NE(new_out_mode, prev_out_mode);
		MEXPECT_NE(new_err_mode, prev_err_mode);

		// Windows actually sets wtext instead of u16text...
		MEXPECT_EQ(new_in_mode, _O_WTEXT);
		MEXPECT_EQ(new_out_mode, _O_WTEXT);
		MEXPECT_EQ(new_err_mode, _O_WTEXT);
#endif
	}

#if FEA_WINDOWS
	EXPECT_EQ(GetConsoleCP(), prev_in_cp);
	EXPECT_EQ(GetConsoleOutputCP(), prev_out_cp);

	// Should have been reset.
	new_in_mode = _setmode(_fileno(stdin), _O_BINARY);
	new_out_mode = _setmode(_fileno(stdout), _O_BINARY);
	new_err_mode = _setmode(_fileno(stderr), _O_BINARY);
	EXPECT_EQ(new_in_mode, prev_in_mode);
	EXPECT_EQ(new_out_mode, prev_out_mode);
	EXPECT_EQ(new_err_mode, prev_err_mode);
	EXPECT_EQ(new_in_mode, _O_TEXT);
	EXPECT_EQ(new_out_mode, _O_TEXT);
	EXPECT_EQ(new_err_mode, _O_TEXT);
#endif

	// Reset
#if FEA_WINDOWS
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	_setmode(_fileno(stdin), _O_TEXT);
	_setmode(_fileno(stdout), _O_TEXT);
	_setmode(_fileno(stderr), _O_TEXT);
#endif
}
} // namespace
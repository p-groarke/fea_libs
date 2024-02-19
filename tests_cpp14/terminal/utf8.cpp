#include <cassert>
#include <cstdio>
#include <fea/terminal/utf8.hpp>
#include <fea/utils/platform.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <iostream>

namespace {
#define MEXPECT_EQ(u, v) \
	if (u != v) \
		throw std::runtime_error { \
			__LINE__ + " : Expected u == v" \
		}

#define MEXPECT_NE(u, v) \
	if (u == v) \
		throw std::runtime_error { \
			__LINE__ + " : Expected u != v" \
		}

TEST(utf8, translate_io) {
#if FEA_WINDOWS
	EXPECT_EQ(int(fea::translation_mode::text), _O_TEXT);
	EXPECT_EQ(int(fea::translation_mode::binary), _O_BINARY);
	EXPECT_EQ(int(fea::translation_mode::wtext), _O_WTEXT);
	EXPECT_EQ(int(fea::translation_mode::u16text), _O_U16TEXT);
	EXPECT_EQ(int(fea::translation_mode::u8text), _O_U8TEXT);
#endif


	fea::translation_resetter tr1
			= fea::translate_io(fea::translation_mode::text);
	fea::unused(tr1);

	{
		fea::translation_resetter tr2
				= fea::translate_io(fea::translation_mode::binary);
		fea::unused(tr2);

#if FEA_WINDOWS
		MEXPECT_EQ(tr2.previous_stdin_mode(), fea::translation_mode::text);
		MEXPECT_EQ(tr2.previous_stdout_mode(), fea::translation_mode::text);
		MEXPECT_EQ(tr2.previous_stderr_mode(), fea::translation_mode::text);

		// Set to new mode to get current.
		fea::translation_resetter tr3
				= fea::translate_io(fea::translation_mode::wtext);
		fea::unused(tr3);
		MEXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::binary);
		MEXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::binary);
		MEXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::binary);

		MEXPECT_NE(tr3.previous_stdin_mode(), tr2.previous_stdin_mode());
		MEXPECT_NE(tr3.previous_stdout_mode(), tr2.previous_stdout_mode());
		MEXPECT_NE(tr3.previous_stderr_mode(), tr2.previous_stderr_mode());
		MEXPECT_NE(tr3.previous_stdin_mode(), tr1.previous_stdin_mode());
		MEXPECT_NE(tr3.previous_stdout_mode(), tr1.previous_stdout_mode());
		MEXPECT_NE(tr3.previous_stderr_mode(), tr1.previous_stderr_mode());
#endif
	}

	// Make sure things are reset. Get previous mode and check.
	fea::translation_resetter tr2
			= fea::translate_io(fea::translation_mode::text);
	fea::unused(tr2);

#if FEA_WINDOWS
	MEXPECT_EQ(tr2.previous_stdin_mode(), fea::translation_mode::text);
	MEXPECT_EQ(tr2.previous_stdout_mode(), fea::translation_mode::text);
	MEXPECT_EQ(tr2.previous_stderr_mode(), fea::translation_mode::text);
#endif

	{
		fea::translation_resetter tr3 = fea::translate_io(
				fea::translation_mode::binary, fea::translation_mode::ignore,
				fea::translation_mode::ignore);
		fea::unused(tr3);

		fea::translation_resetter tr4
				= fea::translate_io(fea::translation_mode::wtext);
		fea::unused(tr4);

#if FEA_WINDOWS
		MEXPECT_EQ(tr4.previous_stdin_mode(), fea::translation_mode::binary);
		MEXPECT_EQ(tr4.previous_stdout_mode(), fea::translation_mode::text);
		MEXPECT_EQ(tr4.previous_stderr_mode(), fea::translation_mode::text);
#endif
	}

	fea::translation_resetter tr3
			= fea::translate_io(fea::translation_mode::text);

#if FEA_WINDOWS
	MEXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::text);
	MEXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::text);
	MEXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::text);
#endif
}

TEST(utf8, utf8_terminal) {
#if FEA_WINDOWS
	SetConsoleCP(28591);
	SetConsoleOutputCP(28591);

	unsigned prev_in_cp = GetConsoleCP();
	unsigned prev_out_cp = GetConsoleOutputCP();
#endif

	fea::translation_resetter tr1
			= fea::translate_io(fea::translation_mode::text);
	fea::unused(tr1);

	{
		auto e = fea::utf8_terminal();
		fea::unused(e);
		std::cout << "Shouldn't assert.\n";

		// Shouldn't have changed.
		fea::translation_resetter tr2
				= fea::translate_io(fea::translation_mode::text);
		fea::unused(tr2);

#if FEA_WINDOWS
		EXPECT_NE(GetConsoleCP(), prev_in_cp);
		EXPECT_NE(GetConsoleOutputCP(), prev_out_cp);

		EXPECT_EQ(tr2.previous_stdin_mode(), fea::translation_mode::text);
		EXPECT_EQ(tr2.previous_stdout_mode(), fea::translation_mode::text);
		EXPECT_EQ(tr2.previous_stderr_mode(), fea::translation_mode::text);
#endif
	}

	// Shouldn't have been reset.
	fea::translation_resetter tr2
			= fea::translate_io(fea::translation_mode::text);
	fea::unused(tr2);

#if FEA_WINDOWS
	// Should be reset.
	EXPECT_EQ(GetConsoleCP(), prev_in_cp);
	EXPECT_EQ(GetConsoleOutputCP(), prev_out_cp);

	EXPECT_EQ(tr2.previous_stdin_mode(), fea::translation_mode::text);
	EXPECT_EQ(tr2.previous_stdout_mode(), fea::translation_mode::text);
	EXPECT_EQ(tr2.previous_stderr_mode(), fea::translation_mode::text);
#endif

	{
		auto e = fea::utf8_terminal(true);
		fea::unused(e);

		// Should have changed,
		fea::translation_resetter tr3
				= fea::translate_io(fea::translation_mode::binary);
		fea::unused(tr3);

#if FEA_WINDOWS
#if FEA_DEBUG
		EXPECT_DEATH(printf("Should assert on windows.\n"), "");
#endif

		// Can't use gtest, since it outputs to cout.
		MEXPECT_NE(GetConsoleCP(), prev_in_cp);
		MEXPECT_NE(GetConsoleOutputCP(), prev_out_cp);

		// Windows actually sets wtext instead of u16text...
		EXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::wtext);
		EXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::wtext);
		EXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::wtext);
#endif
	}

	// Should have been reset.
	fea::translation_resetter tr3
			= fea::translate_io(fea::translation_mode::binary);
	fea::unused(tr3);

#if FEA_WINDOWS
	EXPECT_EQ(GetConsoleCP(), prev_in_cp);
	EXPECT_EQ(GetConsoleOutputCP(), prev_out_cp);

	// Windows actually sets wtext instead of u16text...
	EXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::text);
	EXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::text);
	EXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::text);
#endif

	// Reset
#if FEA_WINDOWS
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif
}
} // namespace
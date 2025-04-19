#include <cassert>
#include <cstdio>
#include <fea/terminal/utf8_io.hpp>
#include <fea/utility/platform.hpp>
#include <fea/utility/unused.hpp>
#include <gtest/gtest.h>
#include <iostream>

namespace {
#define MEXPECT_EQ(u, v) \
	if (u != v) \
	throw std::runtime_error{ __LINE__ + " : Expected u == v" }

#define MEXPECT_NE(u, v) \
	if (u == v) \
	throw std::runtime_error{ __LINE__ + " : Expected u != v" }


TEST(utf8, utf8_io) {
#if FEA_WINDOWS
	unsigned backup_in_cp = GetConsoleCP();
	unsigned backup_out_cp = GetConsoleOutputCP();

	SetConsoleCP(28591);
	SetConsoleOutputCP(28591);

	unsigned prev_in_cp = GetConsoleCP();
	unsigned prev_out_cp = GetConsoleOutputCP();
#endif

	fea::translation_resetter tr1
			= fea::translate_io(fea::translation_mode::text);
	fea::unused(tr1);

	{
		auto e = fea::utf8_io();
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
		auto e = fea::utf8_io(true);
		fea::unused(e);

		// Should have changed,
		fea::translation_resetter tr3
				= fea::translate_io(fea::translation_mode::binary);
		fea::unused(tr3);

#if FEA_WINDOWS
#if FEA_DEBUG && !FEA_VS_EQ(2017)
		EXPECT_DEATH(printf("Should assert on windows.\n"), "");
#endif

		// Can't use gtest, since it outputs to cout.
		MEXPECT_NE(GetConsoleCP(), prev_in_cp);
		MEXPECT_NE(GetConsoleOutputCP(), prev_out_cp);

		std::wcout << int(fea::translation_mode::u8text) << std::endl;
		std::wcout << _O_U8TEXT << std::endl;

		// ~Windows actually sets wtext instead of u16text...~
		// Windows actually does whatever the fuck it wants.
#if FEA_VS_EQ(2017)
		EXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::text);
		EXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::text);
		EXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::text);
#elif FEA_VS_EQ(2019)
		std::wcout << "1 test disables. v142 crashes in 32bits for unknown "
					  "reasons.\n";
#else
		EXPECT_EQ(tr3.previous_stdin_mode(), fea::translation_mode::wtext);
		EXPECT_EQ(tr3.previous_stdout_mode(), fea::translation_mode::wtext);
		EXPECT_EQ(tr3.previous_stderr_mode(), fea::translation_mode::wtext);
#endif
#endif
	}

	{
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
	}

	// Reset
#if FEA_WINDOWS
	SetConsoleCP(backup_in_cp);
	SetConsoleOutputCP(backup_out_cp);
#endif
}

} // namespace
/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "fea/terminal/translate_io.hpp"
#include "fea/utility/platform.hpp"

/*
Terminal utf tools for windows.
Does nothing (but is still callable) on other OSes.
*/

#if FEA_WINDOWS
#include "fea/utility/error.hpp"
#include <limits>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif

namespace fea {
struct codepage_resetter;

#if !FEA_WINDOWS
[[nodiscard]]
inline codepage_resetter utf8_io(bool);
[[nodiscard]]
inline codepage_resetter utf8_io();
struct codepage_resetter {};
#else

// Enables utf8 in windows terminal, as much as possible anyways...
// Does nothing on other platforms.
// In the legacy command prompt, it still won't enable multi-codepoints. But at
// least you'll get the first 16bits of characters when using wide-char prints
// and the likes.
// Capture the returning struct, which will reset the terminal when destroyed.
//
// If you set force_wide to true, the call also enables the terminal translation
// mode for utf16. This is helpful for the legacy command prompt, and will
// assert on any use of non 'w' prefixed input/output c++ functions.
[[nodiscard]]
inline codepage_resetter utf8_io(bool force_wide);

// Enables utf8 in windows terminal, as much as possible anyways...
// Does nothing on other platforms.
// In the legacy command prompt, it still won't enable multi-codepoints. But at
// least you'll get the first 16bits of characters when using wide-char prints
// and the likes.
// Capture the returning struct, which will reset the terminal when destroyed.
[[nodiscard]]
inline codepage_resetter utf8_io();


// Resets codepages on destruction.
struct codepage_resetter {
	// Creates on object that will reset codepage to provided ones on
	// destruction.
	// Returned by the utf8_io call.
	inline codepage_resetter(unsigned in_cp, unsigned out_cp) noexcept;

	// Resets the codepage to stored one.
	inline ~codepage_resetter();

	inline codepage_resetter(codepage_resetter&& other) noexcept;
	inline codepage_resetter& operator=(codepage_resetter&& other) noexcept;

	codepage_resetter() noexcept = default;
	codepage_resetter(const codepage_resetter&) = delete;
	codepage_resetter& operator=(const codepage_resetter&) = delete;

	// Also reset translation on exit.
	inline void reset_translation(translation_resetter&& r);

	// Initial cp sentinel.
	static constexpr unsigned sentinel() noexcept;

private:
	unsigned _in_cp = sentinel();
	unsigned _out_cp = sentinel();

	translation_resetter _trans_reset;
};


#endif
} // namespace fea


// Implementation
namespace fea {
#if !FEA_WINDOWS
codepage_resetter utf8_io(bool) {
	return {};
}
codepage_resetter utf8_io() {
	return {};
}
#else

codepage_resetter utf8_io(bool force_wide) {
	unsigned in_cp_prev = GetConsoleCP();
	if (in_cp_prev == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	unsigned out_cp_prev = GetConsoleOutputCP();
	if (out_cp_prev == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	codepage_resetter ret{ in_cp_prev, out_cp_prev };

	if (SetConsoleCP(CP_UTF8) == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	if (SetConsoleOutputCP(CP_UTF8) == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}

	if (force_wide) {
		ret.reset_translation(translate_io(translation_mode::u16text));
	}
	return ret;
}

codepage_resetter utf8_io() {
	return utf8_io(false);
}
codepage_resetter::codepage_resetter(unsigned in_cp, unsigned out_cp) noexcept
		: _in_cp(in_cp)
		, _out_cp(out_cp) {
}

codepage_resetter::~codepage_resetter() {
	if (_in_cp != sentinel()) {
		if (SetConsoleCP(_in_cp) == 0) {
			fea::error_exit_on_os_error(__FUNCTION__, __LINE__);
		}
	}
	if (_out_cp != sentinel()) {
		if (SetConsoleOutputCP(_out_cp) == 0) {
			fea::error_exit_on_os_error(__FUNCTION__, __LINE__);
		}
	}
}

codepage_resetter::codepage_resetter(codepage_resetter&& other) noexcept
		: _in_cp(other._in_cp)
		, _out_cp(other._out_cp) {
	other._in_cp = sentinel();
	other._out_cp = sentinel();
}

codepage_resetter& codepage_resetter::operator=(
		codepage_resetter&& other) noexcept {
	if (this != &other) {
		_in_cp = other._in_cp;
		_out_cp = other._out_cp;
		other._in_cp = sentinel();
		other._out_cp = sentinel();
	}
	return *this;
}

void codepage_resetter::reset_translation(translation_resetter&& r) {
	_trans_reset = std::move(r);
}

constexpr unsigned codepage_resetter::sentinel() noexcept {
	return (std::numeric_limits<unsigned>::max)();
}
#endif
} // namespace fea

/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include "fea/utils/platform.hpp"

/*
Terminal utf tools for windows.
Does nothing (but is still callable) on other OSes.
*/

#if FEA_WINDOWS
#include "fea/utils/error.hpp"
#include <limits>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif

namespace fea {
#if !FEA_WINDOWS
enum class translation_mode : int {
	ignore,
	text,
	binary,
	wtext,
	u16text,
	u8text,
};
struct translation_resetter {};
FEA_NODISCARD inline translation_resetter translate_io(
		translation_mode, translation_mode, translation_mode) {
	return {};
}
FEA_NODISCARD inline translation_resetter translate_io(translation_mode) {
	return {};
}

struct codepage_resetter {};
FEA_NODISCARD inline codepage_resetter utf8_terminal(bool) {
	return {};
}
FEA_NODISCARD inline codepage_resetter utf8_terminal() {
	return {};
}
#else
enum class translation_mode : int {
	ignore = -1,
	text = _O_TEXT,
	binary = _O_BINARY,
	wtext = _O_WTEXT,
	u16text = _O_U16TEXT,
	u8text = _O_U8TEXT,
};

struct translation_resetter {
	translation_resetter() noexcept = default;
	translation_resetter(int in_mode, int out_mode, int err_mode) noexcept
			: _in_mode(in_mode)
			, _out_mode(out_mode)
			, _err_mode(err_mode) {
	}
	translation_resetter(translation_resetter&& other) noexcept
			: _in_mode(other._in_mode)
			, _out_mode(other._out_mode)
			, _err_mode(other._err_mode) {
		other._in_mode = -1;
		other._out_mode = -1;
		other._err_mode = -1;
	}
	translation_resetter& operator=(translation_resetter&& other) noexcept {
		if (this != &other) {
			_in_mode = other._in_mode;
			_out_mode = other._out_mode;
			_err_mode = other._err_mode;
			other._in_mode = -1;
			other._out_mode = -1;
			other._err_mode = -1;
		}
		return *this;
	}

	translation_resetter(const translation_resetter&) = delete;
	translation_resetter& operator=(const translation_resetter&) = delete;

	~translation_resetter() {
		if (_in_mode != -1) {
			if (_setmode(_fileno(stdin), _in_mode) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}

		if (_out_mode != -1) {
			if (_setmode(_fileno(stdout), _out_mode) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}

		if (_err_mode != -1) {
			if (_setmode(_fileno(stderr), _err_mode) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}
	}

private:
	int _in_mode = -1;
	int _out_mode = -1;
	int _err_mode = -1;
};

// Choose stdin, stdout, stderr
FEA_NODISCARD inline translation_resetter translate_io(translation_mode in_mode,
		translation_mode out_mode, translation_mode err_mode) {
	int in_prev = -1;
	int out_prev = -1;
	int err_prev = -1;

	if (in_mode != translation_mode::ignore) {
		in_prev = _setmode(_fileno(stdin), int(in_mode));
		if (in_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (out_mode != translation_mode::ignore) {
		out_prev = _setmode(_fileno(stdout), int(out_mode));
		if (out_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (err_mode != translation_mode::ignore) {
		err_prev = _setmode(_fileno(stderr), int(err_mode));
		if (err_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	return translation_resetter{ in_prev, out_prev, err_prev };
}

// All set to the same mode.
FEA_NODISCARD inline translation_resetter translate_io(
		translation_mode all_mode) {
	return translate_io(all_mode, all_mode, all_mode);
}

struct codepage_resetter {
	codepage_resetter() noexcept = default;
	codepage_resetter(unsigned in_cp, unsigned out_cp) noexcept
			: _in_cp(in_cp)
			, _out_cp(out_cp) {
	}
	codepage_resetter(codepage_resetter&& other) noexcept
			: _in_cp(other._in_cp)
			, _out_cp(other._out_cp) {
		other._in_cp = sentinel();
		other._out_cp = sentinel();
	}
	codepage_resetter& operator=(codepage_resetter&& other) noexcept {
		if (this != &other) {
			_in_cp = other._in_cp;
			_out_cp = other._out_cp;
			other._in_cp = sentinel();
			other._out_cp = sentinel();
		}
		return *this;
	}

	codepage_resetter(const codepage_resetter&) = delete;
	codepage_resetter& operator=(const codepage_resetter&) = delete;

	~codepage_resetter() {
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

	void reset_translation(translation_resetter&& r) {
		_trans_reset = std::move(r);
	}

	static constexpr unsigned sentinel() {
		return (std::numeric_limits<unsigned>::max)();
	}

private:
	unsigned _in_cp = sentinel();
	unsigned _out_cp = sentinel();

	translation_resetter _trans_reset;
};

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
FEA_NODISCARD inline codepage_resetter utf8_terminal(bool force_wide) {
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
FEA_NODISCARD inline codepage_resetter utf8_terminal() {
	return utf8_terminal(false);
}
#endif
} // namespace fea

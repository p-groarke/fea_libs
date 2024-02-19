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

#if FEA_WINDOWS
#include "fea/utils/error.hpp"
#include <limits>

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#endif

/*
Deals with window's _setmode uglyness. Does nothing on
other platforms (for the time being).
*/

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
struct translation_resetter {
	inline translation_mode previous_stdin_mode() const {
		return translation_mode::ignore;
	}
	inline translation_mode previous_stdout_mode() const {
		return translation_mode::ignore;
	}
	inline translation_mode previous_stderr_mode() const {
		return translation_mode::ignore;
	}
};
FEA_NODISCARD inline translation_resetter translate_io(
		translation_mode, translation_mode, translation_mode) {
	return {};
}
FEA_NODISCARD inline translation_resetter translate_io(translation_mode) {
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
	translation_resetter(translation_mode in_mode, translation_mode out_mode,
			translation_mode err_mode) noexcept
			: _in_mode(in_mode)
			, _out_mode(out_mode)
			, _err_mode(err_mode) {
	}
	translation_resetter(translation_resetter&& other) noexcept
			: _in_mode(other._in_mode)
			, _out_mode(other._out_mode)
			, _err_mode(other._err_mode) {
		other._in_mode = translation_mode::ignore;
		other._out_mode = translation_mode::ignore;
		other._err_mode = translation_mode::ignore;
	}
	translation_resetter& operator=(translation_resetter&& other) noexcept {
		if (this != &other) {
			_in_mode = other._in_mode;
			_out_mode = other._out_mode;
			_err_mode = other._err_mode;
			other._in_mode = translation_mode::ignore;
			other._out_mode = translation_mode::ignore;
			other._err_mode = translation_mode::ignore;
		}
		return *this;
	}

	translation_resetter(const translation_resetter&) = delete;
	translation_resetter& operator=(const translation_resetter&) = delete;

	~translation_resetter() {
		if (_in_mode != translation_mode::ignore) {
			if (fflush(stdin) == EOF) {
				fea::error_exit(
						__FUNCTION__, __LINE__, "Couldn't flush stdin.");
			}
			if (_setmode(_fileno(stdin), int(_in_mode)) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}

		if (_out_mode != translation_mode::ignore) {
			if (fflush(stdout) == EOF) {
				fea::error_exit(
						__FUNCTION__, __LINE__, "Couldn't flush stdout.");
			}
			if (_setmode(_fileno(stdout), int(_out_mode)) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}

		if (_err_mode != translation_mode::ignore) {
			if (fflush(stderr) == EOF) {
				fea::error_exit(
						__FUNCTION__, __LINE__, "Couldn't flush stderr.");
			}
			if (_setmode(_fileno(stderr), int(_err_mode)) == -1) {
				fea::error_exit_on_errno(__FUNCTION__, __LINE__);
			}
		}
	}

	inline translation_mode previous_stdin_mode() const {
		return _in_mode;
	}
	inline translation_mode previous_stdout_mode() const {
		return _out_mode;
	}
	inline translation_mode previous_stderr_mode() const {
		return _err_mode;
	}

private:
	translation_mode _in_mode = translation_mode::ignore;
	translation_mode _out_mode = translation_mode::ignore;
	translation_mode _err_mode = translation_mode::ignore;
};

// Choose stdin, stdout, stderr
FEA_NODISCARD inline translation_resetter translate_io(translation_mode in_mode,
		translation_mode out_mode, translation_mode err_mode) {
	int in_prev = -1;
	int out_prev = -1;
	int err_prev = -1;

	if (in_mode != translation_mode::ignore) {
		if (fflush(stdin) == EOF) {
			fea::maybe_throw(__FUNCTION__, __LINE__, "Couldn't flush stdin.");
		}
		in_prev = _setmode(_fileno(stdin), int(in_mode));
		if (in_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (out_mode != translation_mode::ignore) {
		if (fflush(stdout) == EOF) {
			fea::maybe_throw(__FUNCTION__, __LINE__, "Couldn't flush stdout.");
		}
		out_prev = _setmode(_fileno(stdout), int(out_mode));
		if (out_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (err_mode != translation_mode::ignore) {
		if (fflush(stderr) == EOF) {
			fea::maybe_throw(__FUNCTION__, __LINE__, "Couldn't flush stderr.");
		}
		err_prev = _setmode(_fileno(stderr), int(err_mode));
		if (err_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	return translation_resetter{
		translation_mode(in_prev),
		translation_mode(out_prev),
		translation_mode(err_prev),
	};
}

// All set to the same mode.
FEA_NODISCARD inline translation_resetter translate_io(
		translation_mode all_mode) {
	return translate_io(all_mode, all_mode, all_mode);
}
#endif
} // namespace fea

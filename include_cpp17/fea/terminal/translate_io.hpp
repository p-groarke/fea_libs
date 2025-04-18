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
other platforms.
*/

namespace fea {
struct translation_resetter;

#if !FEA_WINDOWS
enum class translation_mode : int {
	ignore,
	text,
	binary,
	wtext,
	u16text,
	u8text,
};

[[nodiscard]] inline translation_resetter translate_io(
		translation_mode, translation_mode, translation_mode);

[[nodiscard]] inline translation_resetter translate_io(translation_mode);

struct translation_resetter {
	inline translation_mode previous_stdin_mode() const;
	inline translation_mode previous_stdout_mode() const;
	inline translation_mode previous_stderr_mode() const;
};

#else
enum class translation_mode : int {
	ignore = -1,
	text = _O_TEXT,
	binary = _O_BINARY,
	wtext = _O_WTEXT,
	u16text = _O_U16TEXT,
	u8text = _O_U8TEXT,
};

// Enable windows io translation mode (_setmode).
// This fixes some issues when reading piped text or when outputting to windows
// terminal.
// Choose stdin, stdout, stderr
[[nodiscard]]
inline translation_resetter translate_io(translation_mode in_mode,
		translation_mode out_mode, translation_mode err_mode);

// Enable windows io translation mode (_setmode).
// This fixes some issues when reading piped text or when outputting to windows
// terminal.
// All set to the same mode.
[[nodiscard]]
inline translation_resetter translate_io(translation_mode all_mode);


// Resets translation mode on destruction.
struct translation_resetter {
	// Creates an object which will reset the translation mode on destruction.
	// Created by the translate_io call.
	inline translation_resetter(translation_mode in_mode,
			translation_mode out_mode, translation_mode err_mode) noexcept;

	// Resets the translation mode to stored one.
	inline ~translation_resetter();

	inline translation_resetter(translation_resetter&& other) noexcept;
	inline translation_resetter& operator=(
			translation_resetter&& other) noexcept;

	translation_resetter() noexcept = default;
	translation_resetter(const translation_resetter&) = delete;
	translation_resetter& operator=(const translation_resetter&) = delete;

	// Returns previous stdin mode, before this resetter took effect.
	inline translation_mode previous_stdin_mode() const;

	// Returns previous stdout mode, before this resetter took effect.
	inline translation_mode previous_stdout_mode() const;

	// Returns previous stderr mode, before this resetter took effect.
	inline translation_mode previous_stderr_mode() const;

private:
	translation_mode _in_mode = translation_mode::ignore;
	translation_mode _out_mode = translation_mode::ignore;
	translation_mode _err_mode = translation_mode::ignore;
};
#endif
} // namespace fea

#include "imp/translate_io.imp.hpp"

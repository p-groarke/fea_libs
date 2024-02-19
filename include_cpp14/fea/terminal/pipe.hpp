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
#include "fea/terminal/utf8.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/scope.hpp"
#include "fea/utils/unused.hpp"

#include <iostream>
#include <string>

/**
 * If there is any text in the application pipe, reads it in the output string.
 */

namespace fea {
namespace detail {
template <class CinT, class StringT>
inline void read_pipe_text(CinT& mcin, StringT& out, bool clear) {
	auto e = fea::make_on_exit([&, clear]() {
		// Clear and flush pipe.
		if (clear) {
			mcin.clear();
		}
	});

	// Check if we have anything in cin.
	mcin.seekg(0, mcin.end);
	std::streamoff cin_count = mcin.tellg();
	mcin.seekg(0, mcin.beg);

	if (cin_count <= 0) {
		return;
	}

	StringT line;
	while (std::getline(mcin, line)) {
		out.insert(out.end(), line.begin(), line.end());
		out += L'\n';
	}
}
} // namespace detail


// If there is any text in application pipe, read it.
// Clears the pipe if clear_pipe is true.
inline std::wstring wread_pipe_text(bool clear_pipe) {
	// To fix pipe input, use U8TEXT (and not U16).
	fea::translation_resetter tr
			= fea::translate_io(fea::translation_mode::u8text);
	fea::unused(tr);

	std::wstring ret;
	detail::read_pipe_text(std::wcin, ret, clear_pipe);
	return ret;
}

// If there is any text in application pipe, read it.
// Clears the pipe.
inline std::wstring wread_pipe_text() {
	return fea::wread_pipe_text(true);
}

// If there is any text in application pipe, read it.
// Clears the pipe if clear_pipe is true.
inline std::string read_pipe_text(bool clear_pipe) {
	std::string ret;
	detail::read_pipe_text(std::cin, ret, clear_pipe);
	return ret;
}

// If there is any text in application pipe, read it.
// Clears the pipe.
inline std::string read_pipe_text() {
	return fea::read_pipe_text(true);
}

} // namespace fea

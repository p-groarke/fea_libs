/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/terminal/translation_mode.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/scope.hpp"
#include "fea/utils/unused.hpp"

#include <codecvt>
#include <iostream>
#include <locale>
#include <string>

#if FEA_WINDOWS
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

// The standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#if FEA_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4996)
#elif FEA_MACOS
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

/*
Cross-platform stdin pipe helpers.
*/

namespace fea {
inline size_t available_pipe_bytes();

namespace detail {
template <class CinT, class StringT>
inline void read_pipe_text(CinT& mcin, StringT& out) {
	auto e = fea::make_on_exit([&]() {
		// Clear pipe bits.
		mcin.clear();
	});

	size_t cin_count = fea::available_pipe_bytes();
	if (cin_count == 0) {
		return;
	}

	out.reserve((cin_count / sizeof(typename StringT::value_type)) + 1);
	StringT line;
	while (std::getline(mcin, line)) {
		out.insert(out.end(), line.begin(), line.end());
		out += L'\n';
	}
}
} // namespace detail


// Disables syncing with C io functions. Makes std::cout and friends fast.
inline void fast_iostreams() {
	std::cin.sync_with_stdio(false);
	std::cout.sync_with_stdio(false);
	std::cerr.sync_with_stdio(false);
	std::clog.sync_with_stdio(false);
	std::wcin.sync_with_stdio(false);
	std::wcout.sync_with_stdio(false);
	std::wcerr.sync_with_stdio(false);
	std::wclog.sync_with_stdio(false);
}

// A non-blocking function that returns the number of bytes available in stdin.
size_t available_pipe_bytes() {
	size_t ret = 0;

#if FEA_WINDOWS
	HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdin_handle == INVALID_HANDLE_VALUE) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}

	switch (GetFileType(stdin_handle)) {
	case FILE_TYPE_CHAR: {
		// Unsupported. Please send me a use-case / example.
	} break;
	case FILE_TYPE_DISK: {
		LARGE_INTEGER byte_size;
#if FEA_32BIT
		byte_size.LowPart = 0;
#else
		byte_size.QuadPart = 0;
#endif
		if (GetFileSizeEx(stdin_handle, &byte_size) == 0) {
			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
		}
#if FEA_32BIT
		ret = size_t(byte_size.LowPart);
#else
		ret = size_t(byte_size.QuadPart);
#endif
	} break;
	case FILE_TYPE_PIPE: {
		// For some obscure reason, we need to readfile first.
		char buf[] = { 0 };
		if (ReadFile(stdin_handle, &buf, 0, nullptr, nullptr) == 0) {
			SetLastError(0);
			return 0;
		}

		// Now peek size.
		unsigned long avail = 0;
		if (PeekNamedPipe(stdin_handle, nullptr, 0, nullptr, &avail, nullptr)
				== 0) {
			fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
		}
		ret = size_t(avail);
	} break;
	case FILE_TYPE_REMOTE: {
		// Unsupported. Please send me a use-case / example.
	} break;
	default: {
		// GetLastError returns NO_ERROR on valid unknown.
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	} break;
	}
#else
	int n = 0;
	if (ioctl(fileno(stdin), FIONREAD, &n) != 0) {
		fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
	}
	ret = size_t(n);
#endif

	return ret;
}

// If there is any text in application pipe, read it.
// Clears the pipe if clear_pipe is true.
inline std::wstring wread_pipe_text() {
	std::wstring ret;

#if FEA_WINDOWS
	// To fix pipe input, use U8TEXT (and not U16).
	fea::translation_resetter tr
			= fea::translate_io(fea::translation_mode::u8text);
	fea::unused(tr);

	detail::read_pipe_text(std::wcin, ret);
#else
	// wcin is borked, use cin and convert to wstring (utf32).
	std::string temp;
	detail::read_pipe_text(std::cin, temp);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	ret = convert.from_bytes(temp);
#endif
	return ret;
}

// If there is any text in application pipe, read it.
// Clears the pipe if clear_pipe is true.
inline std::string read_pipe_text() {
	std::string ret;
	detail::read_pipe_text(std::cin, ret);
	return ret;
}
} // namespace fea

#if FEA_WINDOWS
#pragma warning(pop)
#elif FEA_MACOS
#pragma clang diagnostic pop
#endif

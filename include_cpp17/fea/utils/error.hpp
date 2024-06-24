/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <string>
#include <system_error>

#if FEA_WINDOWS
#include <windows.h>
#else
#include <errno.h>
#endif

namespace fea {
// Cross-platform helper to get last OS error.
// GetLastError on Windows, errno on anything else.
[[nodiscard]]
inline std::error_code last_os_error();

// Some windows function report in errno. Use this to force errno on windows.
[[nodiscard]]
inline std::error_code last_errno_error();

// Prints error message.
inline void print_error_message(
		const char* func_name, size_t line, const std::error_code& ec);

// Throws if FEA_NOTHROW is not defined, else exits with error code.
// Provide __FUNCTION__, __LINE__, std::error_code.
inline void maybe_throw(
		const char* func_name, size_t line, const std::error_code& ec);

// If there is a system error, throws if FEA_NOTHROW is not defined, else exits
// with error code. Prints last error message.
// Provide __FUNCTION__, __LINE__.
// Uses GetLastError on windows, errno on posix.
inline void maybe_throw_on_os_error(const char* func_name, size_t line);

// If there is a system error, throws if FEA_NOTHROW is not defined, else exits
// with error code. Prints last error message.
// Provide __FUNCTION__, __LINE__.
// Uses errno on all platforms.
inline void maybe_throw_on_errno(const char* func_name, size_t line);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__, "your message".
inline void error_exit(
		const char* func_name, size_t line, const std::error_code& ec);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__.
// Uses GetLastError on windows, errno on posix.
inline void error_exit_on_os_error(const char* func_name, size_t line);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__.
// Uses errno on all platforms.
inline void error_exit_on_errno(const char* func_name, size_t line);

} // namespace fea

#include "imp/error.imp.hpp"

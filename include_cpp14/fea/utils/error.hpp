/*
BSD 3-Clause License

Copyright (c) 2022, Philippe Groarke
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
inline std::error_code last_os_error() {
#if FEA_WINDOWS
	return std::error_code{ int(GetLastError()), std::system_category() };
#else
	return std::error_code{ errno, std::system_category() };
#endif
}

// Prints error message.
// Throws if FEA_NOTHROW is not defined, else exits with error code.
// Provide __FUNCTION__, __LINE__, std::error_code.
inline void maybe_throw(
		const char* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	fea::print_error_message(func_name, line, msg);
	assert(false);

#if !defined(FEA_NOTHROW)
	throw std::system_error{ ec,
		std::string{ func_name } + "(" + std::to_string(line) + ")" + " : "
				+ msg };
#else
	std::exit(EXIT_FAILURE);
#endif
}
} // namespace fea

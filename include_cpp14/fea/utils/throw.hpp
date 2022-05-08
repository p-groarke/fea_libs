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
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>

/*
Exception Helpers

Throw on builds where FEA_NOTHROW is not defined.
Print exceptions message and terminate if FEA_NOTHROW is defined.

Always asserts (for better multi-threaded breaking).
Always prints the error message to help with ci or other environments
where getting the error message may not be trivial.

TODO ?
You may pass in __FILE__, __FUNCTION__, __LINE__ with the 'FEA_ERR_INFO' macro.
ex : fea::maybe_throw(FEA_ERR_INFO, "my message");
*/

//#define FEA_ERR_INFO __FILE__, __FUNCTION__, __LINE__

namespace std {
// Most popular ones.
class runtime_error;
class invalid_argument;
class out_of_range;
} // namespace std

#if !defined(FEA_NOTHROW)
#include <stdexcept>
#endif

namespace fea {
// Throws if FEA_NOTHROW is not defined, else prints the error message and
// exits with error code.
// Provide __FUNCTION__, __LINE__, "your message".
template <class Ex = std::runtime_error>
inline void maybe_throw(
		const char* func_name, size_t line, const std::string& message) {

	fprintf(stderr, "%s(%zu) : %s\n", func_name, line, message.c_str());
	assert(false);

#if !defined(FEA_NOTHROW)
	throw Ex{ std::string{ func_name } + "(" + std::to_string(line) + ")"
		+ " : " + message };
#else
	std::exit(EXIT_FAILURE);
#endif
}

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__, "your message".
inline void error_exit(
		const char* func_name, size_t line, const std::string& message) {
	fprintf(stderr, "%s(%zu) : %s\n", func_name, line, message.c_str());
	assert(false);
	std::exit(EXIT_FAILURE);
}

// Prints error message.
// Provide __FUNCTION__, __LINE__, "your message".
inline void error_message(
		const char* func_name, size_t line, const std::string& message) {
	fprintf(stderr, "%s(%zu) : %s\n", func_name, line, message.c_str());
}
} // namespace fea

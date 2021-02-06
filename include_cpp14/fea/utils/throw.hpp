/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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

#if !defined(FEA_NOTHROW)
#include <stdexcept>
#endif

namespace fea {
// Throws if FEA_NOTHROW is not defined, else prints the error message and
// exits with error code.
inline void maybe_throw(const char* func_name, const std::string& message) {
	// Always assert.
	assert(false);

#if !defined(FEA_NOTHROW)
	throw std::runtime_error{ std::string{ func_name } + " : " + message };
#else
	fprintf(stderr, "%s : %s\n", func_name, message.c_str());
	std::exit(EXIT_FAILURE);
#endif
}

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
inline void error_exit(const char* func_name, const std::string& message) {
	// Always assert.
	assert(false);
	fprintf(stderr, "%s : %s\n", func_name, message.c_str());
	std::exit(EXIT_FAILURE);
}
} // namespace fea

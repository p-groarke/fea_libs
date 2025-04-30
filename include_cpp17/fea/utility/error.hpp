/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
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
#include "fea/string/conversions.hpp"
#include "fea/utility/platform.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <system_error>

#if FEA_WINDOWS
#include <windows.h>
#else
#include <errno.h>
#endif

namespace std {
// Most popular ones.
class runtime_error;
class invalid_argument;
class out_of_range;
} // namespace std

#if !FEA_NOTHROW
#include <stdexcept>
#endif


/*
Error and Exception Helpers

Throw on builds where FEA_NOTHROW is not defined.
Print exceptions message and terminate if FEA_NOTHROW is defined.

Always asserts (for better multi-threaded breaking).
Always prints the error message to help with ci or other environments
where getting the error message may not be trivial.
*/

namespace fea {
// Cross-platform helper to get last OS error.
// GetLastError on Windows, errno on anything else.
[[nodiscard]]
inline std::error_code last_os_error();

// Some windows function report in errno. Use this to force errno on windows.
[[nodiscard]]
inline std::error_code last_errno_error();

// Prints error message.
// Provide __FUNCTION__, __LINE__, "your message".
inline void print_error_message(
		const char* func_name, size_t line, const std::string& message);
inline void print_error_message(
		const wchar_t* func_name, size_t line, const std::wstring& message);

// Prints error message.
// Provide __FUNCTION__, __LINE__, std::error_code.
inline void print_error_message(
		const char* func_name, size_t line, const std::error_code& ec);
inline void print_error_message(
		const wchar_t* func_name, size_t line, const std::error_code& ec);

// Prints error message.
// Throws if FEA_NOTHROW is not defined, else exits with error code.
// Provide __FUNCTION__, __LINE__, "your message".
template <class Ex = std::runtime_error>
void maybe_throw(
		const char* func_name, size_t line, const std::string& message);
template <class Ex = std::runtime_error>
void maybe_throw(
		const wchar_t* func_name, size_t line, const std::wstring& message);

// Throws if FEA_NOTHROW is not defined, else exits with error code.
// Provide __FUNCTION__, __LINE__, std::error_code.
inline void maybe_throw(
		const char* func_name, size_t line, const std::error_code& ec);
inline void maybe_throw(
		const wchar_t* func_name, size_t line, const std::error_code& ec);

// If there is a system error, throws if FEA_NOTHROW is not defined, else exits
// with error code. Prints last error message.
// Provide __FUNCTION__, __LINE__.
// Uses GetLastError on windows, errno on posix.
inline void maybe_throw_on_os_error(const char* func_name, size_t line);
inline void maybe_throw_on_os_error(const wchar_t* func_name, size_t line);

// If there is a system error, throws if FEA_NOTHROW is not defined, else exits
// with error code. Prints last error message.
// Provide __FUNCTION__, __LINE__.
// Uses errno on all platforms.
inline void maybe_throw_on_errno(const char* func_name, size_t line);
inline void maybe_throw_on_errno(const wchar_t* func_name, size_t line);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__, "your message".
inline void error_exit(
		const char* func_name, size_t line, const std::string& message);
inline void error_exit(
		const wchar_t* func_name, size_t line, const std::wstring& message);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__, "your message".
inline void error_exit(
		const char* func_name, size_t line, const std::error_code& ec);
inline void error_exit(
		const wchar_t* func_name, size_t line, const std::error_code& ec);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__.
// Uses GetLastError on windows, errno on posix.
inline void error_exit_on_os_error(const char* func_name, size_t line);
inline void error_exit_on_os_error(const wchar_t* func_name, size_t line);

// Prints message and exits with error code.
// Use this when you absolutely can't throw (from destructors for example).
// Provide __FUNCTION__, __LINE__.
// Uses errno on all platforms.
inline void error_exit_on_errno(const char* func_name, size_t line);
inline void error_exit_on_errno(const wchar_t* func_name, size_t line);
} // namespace fea


// Implementation
namespace fea {
std::error_code last_os_error() {
#if FEA_WINDOWS
	return std::error_code{ int(GetLastError()), std::system_category() };
#else
	return std::error_code{ errno, std::system_category() };
#endif
}

std::error_code last_errno_error() {
	return std::error_code{ errno, std::system_category() };
}


void print_error_message(
		const char* func_name, size_t line, const std::string& message) {
	fprintf(stderr, "%s(%zu) : %s\n", func_name, line, message.c_str());
}

void print_error_message(
		const wchar_t* func_name, size_t line, const std::wstring& message) {
	fwprintf(stderr, L"%s(%zu) : %s\n", func_name, line, message.c_str());
}


void print_error_message(
		const char* func_name, size_t line, const std::error_code& ec) {
	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	fea::print_error_message(func_name, line, msg);
}

void print_error_message(
		const wchar_t* func_name, size_t line, const std::error_code& ec) {
	std::wstring msg = L"Error Code " + std::to_wstring(ec.value()) + L". "
					 + fea::utf8_to_utf16_w(ec.message());
	fea::print_error_message(func_name, line, msg);
}


template <class Ex /*= std::runtime_error*/>
void maybe_throw(
		const char* func_name, size_t line, const std::string& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);

#if !FEA_NOTHROW
	throw Ex{ std::string{ func_name } + "(" + std::to_string(line) + ")"
			  + " : " + message };
#else
	std::exit(EXIT_FAILURE);
#endif
}

template <class Ex /*= std::runtime_error*/>
void maybe_throw(
		const wchar_t* func_name, size_t line, const std::wstring& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);

#if !FEA_NOTHROW
	throw Ex{ fea::utf16_to_utf8(func_name) + "(" + std::to_string(line) + ")"
			  + " : " + fea::utf16_to_utf8(message) };
#else
	std::exit(EXIT_FAILURE);
#endif
}


void maybe_throw(
		const char* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);

#if !FEA_NOTHROW
	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	throw std::system_error{ ec, std::string{ func_name } + "("
										 + std::to_string(line) + ")" + " : "
										 + msg };
#else
	std::exit(EXIT_FAILURE);
#endif
}

void maybe_throw(
		const wchar_t* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);

#if !FEA_NOTHROW
	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	throw std::system_error{ ec, fea::utf16_to_utf8(func_name) + "("
										 + std::to_string(line) + ")" + " : "
										 + msg };
#else
	std::exit(EXIT_FAILURE);
#endif
}


void maybe_throw_on_os_error(const char* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_os_error());
}

void maybe_throw_on_os_error(const wchar_t* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_os_error());
}


void maybe_throw_on_errno(const char* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_errno_error());
}

void maybe_throw_on_errno(const wchar_t* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_errno_error());
}


void error_exit(
		const char* func_name, size_t line, const std::string& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);
	std::exit(EXIT_FAILURE);
}

void error_exit(
		const wchar_t* func_name, size_t line, const std::wstring& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);
	std::exit(EXIT_FAILURE);
}


void error_exit(const char* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);
	std::exit(EXIT_FAILURE);
}

void error_exit(
		const wchar_t* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);
	std::exit(EXIT_FAILURE);
}


void error_exit_on_os_error(const char* func_name, size_t line) {
	error_exit(func_name, line, fea::last_os_error());
}

void error_exit_on_os_error(const wchar_t* func_name, size_t line) {
	error_exit(func_name, line, fea::last_os_error());
}


void error_exit_on_errno(const char* func_name, size_t line) {
	error_exit(func_name, line, fea::last_errno_error());
}

void error_exit_on_errno(const wchar_t* func_name, size_t line) {
	error_exit(func_name, line, fea::last_errno_error());
}
} // namespace fea

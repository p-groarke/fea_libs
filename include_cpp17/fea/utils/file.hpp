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
#include "fea/string/string.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

namespace fea {
// Returns the executable's directory. You must provide argv[0].
[[nodiscard]]
inline std::filesystem::path executable_dir(const char* argv0);

// Returns the executable's directory. You must provide argv[0].
[[nodiscard]]
inline std::filesystem::path executable_dir(const wchar_t* argv0);

// Cross-platform "safe" fopen.
[[nodiscard]]
inline std::FILE* fopen(const std::filesystem::path& path, const char* mode);

// Returns the full size of the c filestream. Rewinds the stream.
[[nodiscard]]
inline size_t file_size(std::FILE* ifs);

// Analog to std::rewind for fstreams.
// Clears error state and returns to filestream beginning.
template <class IFStream>
void rewind(IFStream& fs);

// Returns the full size of the filestream. Leaves the stream at the beginning.
template <class IFStream>
[[nodiscard]]
size_t file_size(IFStream& ifs);

// Calls your function for every line in a text file. Removes linefeeds.
// Pass in void(String&&)
template <class IFStream, class String, class Func>
bool basic_read_text_file(const std::filesystem::path& fpath, Func&& func);

// Calls your function for every line in a text file. Removes linefeeds.
// Pass in void(std::string&&)
template <class Func>
bool read_text_file(const std::filesystem::path& fpath, Func&& func);

// Calls your function for every line in a text file. Removes linefeeds.
// Pass in void(std::wstring&&)
template <class Func>
bool wread_text_file(const std::filesystem::path& fpath, Func&& func);

// Opens the text file as unsigned, and stores it in out.
// The output is a text blob (no linefeeds).
template <class IFStream, class String, class UInt>
bool basic_open_text_file(
		const std::filesystem::path& fpath, std::vector<UInt>& out);

// Opens the text file as unsigned, and stores it in out.
// The output is a text blob (no linefeeds).
inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<uint8_t>& out);

// Opens the text file as unsigned, and stores it in out.
// The output is a text blob (no linefeeds).
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<uint16_t>& out);

// Opens text file and files the vector of vector with each line converted to
// unsigned.
template <class IFStream, class String, class UInt>
bool basic_open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<UInt>>& out);

// Opens text file and files the vector of vector with each line converted to
// unsigned.
inline bool open_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint8_t>>& out);

// Opens text file and files the vector of vector with each line converted to
// unsigned.
inline bool wopen_text_file(const std::filesystem::path& fpath,
		std::vector<std::vector<uint16_t>>& out);

// Opens the text file and stores it in out.
// The output is a text blob (no linefeeds).
template <class IFStream, class UInt, class String>
bool basic_open_text_file(const std::filesystem::path& fpath, String& out);

// Opens the text file and stores it in out.
// The output is a text blob (no linefeeds).
inline bool open_text_file(
		const std::filesystem::path& fpath, std::string& out);

// Opens the text file and stores it in out.
// The output is a text blob (no linefeeds).
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::wstring& out);

// Opens the text file and each line in the vector.
template <class IFStream, class String, class Alloc,
		template <class, class> class Vec>
bool basic_open_text_file(
		const std::filesystem::path& fpath, Vec<String, Alloc>& out);

// Opens the text file and each line in the vector.
inline bool open_text_file(
		const std::filesystem::path& fpath, std::vector<std::string>& out);

// Opens the text file and each line in the vector.
inline bool wopen_text_file(
		const std::filesystem::path& fpath, std::vector<std::wstring>& out);

// Opens the text file as-is (without parsing) and stores it in out.
// Fastest option.
template <class IFStream, class String>
bool open_text_file_raw(const std::filesystem::path& fpath, String& out);

// Opens the text file as-is (without parsing) and stores it in out.
// Fastest option.
inline bool open_text_file_raw(
		const std::filesystem::path& fpath, std::string& out);

// Opens the text file as-is (without parsing) and stores it in out.
// Fastest option.
inline bool wopen_text_file_raw(
		const std::filesystem::path& fpath, std::wstring& out);

// Opens binary file and stores bytes in vector.
inline bool open_binary_file(
		const std::filesystem::path& f, std::vector<uint8_t>& out);

// Utf encoding formats for file open functions that parse / deduce utf.
enum class text_encoding {
	utf32be,
	utf32le,
	utf16be,
	utf16le,
	utf8,
	count,
};

// Try to detect a string's character encoding.
// Based on :
// https://www.codeproject.com/Tips/672470/Simple-Character-Encoding-Detection
[[nodiscard]]
inline text_encoding detect_encoding(const std::string& str);

// Converts input string with provided encoding into utf32.
// Takes into consideration little or big endianness.
// The output is a little-endian utf32 string.
inline bool file_string_to_utf32(const std::string& input_str,
		text_encoding encoding, std::u32string& output_str);

// Open a utf text file.
// Infers utf type if BOM is present.
// If not, tries to detect encoding of the file.
[[nodiscard]]
inline std::u32string open_text_file_with_bom(std::ifstream& src);
} // namespace fea

#include "imp/file.imp.hpp"

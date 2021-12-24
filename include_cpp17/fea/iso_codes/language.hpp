/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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

#if FEA_ARCH >= 64
#include "fea/iso_codes/language_database.hpp"

#include <array>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

/*
ISO 639-1, 639-2B, 639-2T, 639-3 lookups.
Codes attribution : https://www.iso639-3.sil.org
*/

namespace fea {
namespace detail {
constexpr inline uint16_t compress_3char_code(std::string_view code) {
	assert(code.size() == 3);
	return uint16_t(code[2] - 'a') << 10 | uint16_t(code[1] - 'a') << 5
			| uint16_t(code[0] - 'a');
}

constexpr inline uint16_t compress_2char_code(std::string_view code) {
	assert(code.size() == 2);
	return uint16_t(code[1] - 'a') << 5 | uint16_t(code[0] - 'a');
}
} // namespace detail

// Returns the language associated with the provided ISO 639-3 code.
// Code must be 3 characters and lowercase.
inline const iso_639_lang& iso_639_3_lookup(std::string_view code) {
	uint16_t idx
			= detail::iso_639_3_code_to_id[detail::compress_3char_code(code)];
	assert(idx != (std::numeric_limits<uint16_t>::max)());
	return detail::iso_639_languages[idx];
}

// Returns the language associated with the provided ISO 639-2b code.
// Code must be 3 characters and lowercase.
inline const iso_639_lang& iso_639_2b_lookup(std::string_view code) {
	uint16_t idx
			= detail::iso_639_2b_code_to_id[detail::compress_3char_code(code)];
	assert(idx != (std::numeric_limits<uint16_t>::max)());
	return detail::iso_639_languages[idx];
}

// Returns the language associated with the provided ISO 639-2t code.
// Code must be 3 characters and lowercase.
inline const iso_639_lang& iso_639_2t_lookup(std::string_view code) {
	uint16_t idx
			= detail::iso_639_2t_code_to_id[detail::compress_3char_code(code)];
	assert(idx != (std::numeric_limits<uint16_t>::max)());
	return detail::iso_639_languages[idx];
}

// Returns the language associated with the provided ISO 639-1 code.
// Code must be 2 characters and lowercase.
inline const iso_639_lang& iso_639_1_lookup(std::string_view code) {
	uint16_t idx
			= detail::iso_639_1_code_to_id[detail::compress_2char_code(code)];
	assert(idx != (std::numeric_limits<uint16_t>::max)());
	return detail::iso_639_languages[idx];
}
} // namespace fea
#endif

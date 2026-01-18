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
#include "fea/language/iso_639_lang.hpp"
#include "fea/string/string.hpp"
#include "fea/string/string_literal.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <vector>

/*
A collection of language related functions and data.

ISO 639-1, 639-2B, 639-2T, 639-3 lookups.
Codes attribution : https://www.iso639-3.sil.org
*/

namespace fea {
// Check if your code is valid.
[[nodiscard]]
inline bool iso_639_3_valid(std::string_view code) noexcept;

// Check if your code is valid.
[[nodiscard]]
inline bool iso_639_2b_valid(std::string_view code) noexcept;

// Check if your code is valid.
[[nodiscard]]
inline bool iso_639_2t_valid(std::string_view code) noexcept;

// Check if your code is valid.
[[nodiscard]]
inline bool iso_639_1_valid(std::string_view code) noexcept;


// Returns the language associated with the provided ISO 639-3 code.
// Code must be 3 characters and lowercase.
[[nodiscard]]
inline const iso_639_lang& iso_639_3_lookup(std::string_view code) noexcept;

// Returns the language associated with the provided ISO 639-2b code.
// Code must be 3 characters and lowercase.
[[nodiscard]]
inline const iso_639_lang& iso_639_2b_lookup(std::string_view code) noexcept;

// Returns the language associated with the provided ISO 639-2t code.
// Code must be 3 characters and lowercase.
[[nodiscard]]
inline const iso_639_lang& iso_639_2t_lookup(std::string_view code) noexcept;

// Returns the language associated with the provided ISO 639-1 code.
// Code must be 2 characters and lowercase.
[[nodiscard]]
inline const iso_639_lang& iso_639_1_lookup(std::string_view code) noexcept;


namespace en {
// Returns the alphabet letters, sorted by frequency.
// Wikipedia original source.
// https://web.archive.org/web/20210304152631/http://en.algoritmy.net/article/40379/Letter-frequency-English
[[nodiscard]]
constexpr std::span<const char> letters() noexcept;

// Returns the absolute letter frequency percentage. Wikipedia source.
// Aka, the frequency in all of the english language.
// Expects ascii letter.
[[nodiscard]]
constexpr double letter_frequency(char l) noexcept;

// Returns the absolute frequency percentage. Wikipedia source.
// Aka, the frequency in all of the english language.
// Expects ascii letter.
[[nodiscard]]
constexpr double letter_frequency(uint8_t l) noexcept;

// Returns the local letter frequency at word position. Norvig source.
// Aka, how often does this letter appear in a word at given position.
// Expects ascii letter.
[[nodiscard]]
constexpr double letter_frequency(uint8_t l) noexcept;


// Returns a small english dictionary of popular words.
// Peter Norvig's dataset from google corpus analysis, trimmed.
// https://norvig.com/mayzner.html
// Unique, lower-case, sorted by popularity.
[[nodiscard]]
constexpr std::span<const std::string_view> dictionary() noexcept;

// Returns a popularity sorted list of bigrams. Norvig's dataset.
// Unique, lower-case, sorted by popularity.
[[nodiscard]]
constexpr std::span<const std::string_view> bigrams() noexcept;

// Returns the bigram frequency percentage. Norvig's dataset.
// Expects lower-case letter pair.
[[nodiscard]]
constexpr double bigram_frequency(std::string_view bigram) noexcept;
} // namespace en
} // namespace fea


// Data
#include "english_db.hpp"
#include "iso_639_db.hpp"


// Implementation
namespace fea {
namespace detail {
constexpr uint16_t compress_3char_code(std::string_view code) noexcept {
	assert(code.size() == 3);
	return uint16_t(code[2] - 'a') << 10 | uint16_t(code[1] - 'a') << 5
		 | uint16_t(code[0] - 'a');
}

constexpr uint16_t compress_2char_code(std::string_view code) noexcept {
	assert(code.size() == 2);
	return uint16_t(code[1] - 'a') << 5 | uint16_t(code[0] - 'a');
}
} // namespace detail
} // namespace fea

namespace fea {
bool iso_639_3_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_3_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_2b_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_2b_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_2t_valid(std::string_view code) noexcept {
	if (code.size() != 3) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_2t_code_to_id[detail::compress_3char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

bool iso_639_1_valid(std::string_view code) noexcept {
	if (code.size() != 2) {
		return false;
	}
	uint16_t idx
			= detail::iso_639_1_code_to_id[detail::compress_2char_code(code)];
	return (idx != (std::numeric_limits<uint16_t>::max)());
}

const iso_639_lang& iso_639_3_lookup(std::string_view code) noexcept {
	assert(iso_639_3_valid(code));
	uint16_t idx
			= detail::iso_639_3_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_2b_lookup(std::string_view code) noexcept {
	assert(iso_639_2b_valid(code));
	uint16_t idx
			= detail::iso_639_2b_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_2t_lookup(std::string_view code) noexcept {
	assert(iso_639_2t_valid(code));
	uint16_t idx
			= detail::iso_639_2t_code_to_id[detail::compress_3char_code(code)];
	return detail::iso_639_languages[idx];
}

const iso_639_lang& iso_639_1_lookup(std::string_view code) noexcept {
	assert(iso_639_1_valid(code));
	uint16_t idx
			= detail::iso_639_1_code_to_id[detail::compress_2char_code(code)];
	return detail::iso_639_languages[idx];
}

namespace en {
constexpr std::span<const char> letters() noexcept {
	return std::span{ detail::letters.data(), detail::letters.size() };
}

constexpr double letter_frequency(char l) noexcept {
	return letter_frequency(uint8_t(l));
}

constexpr double letter_frequency(uint8_t l) noexcept {
	return detail::letter_frequencies[size_t(l)];
}

constexpr std::span<const std::string_view> dictionary() noexcept {
	return std::span{ detail::english_dic.data(), detail::english_dic.size() };
}

constexpr std::span<const std::string_view> bigrams() noexcept {
	return std::span{ detail::bigrams.data(), detail::bigrams.size() };
}

constexpr double bigram_frequency(std::string_view bigram) noexcept {
	assert(bigram.size() == 2u);
	assert(std::all_of(bigram.begin(), bigram.end(),
			[](char c) { return fea::is_lower_letter_ascii(c); }));

	size_t idx = detail::bigram_idx(bigram);
	assert(idx < detail::bigram_frequencies.size());
	return detail::bigram_frequencies[idx];
}
} // namespace en
} // namespace fea

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

inline uint16_t compress_3char_code(std::string_view code) {
	struct compress_t {
		uint16_t ch1 : 5;
		uint16_t ch2 : 5;
		uint16_t ch3 : 5;
		uint16_t unused : 1 = 0;
	};
	static_assert(sizeof(compress_t) == 2, "");

	compress_t comp{
		uint16_t(code[0] - 'a'),
		uint16_t(code[1] - 'a'),
		uint16_t(code[2] - 'a'),
	};
	return *reinterpret_cast<uint16_t*>(&comp);
}

inline uint16_t compress_2char_code(std::string_view code) {
	struct compress_t {
		uint16_t ch1 : 5;
		uint16_t ch2 : 5;
		uint16_t unused : 6 = 0;
	};
	static_assert(sizeof(compress_t) == 2, "");

	compress_t comp{
		uint16_t(code[0] - 'a'),
		uint16_t(code[1] - 'a'),
	};
	return *reinterpret_cast<uint16_t*>(&comp);
}
} // namespace fea

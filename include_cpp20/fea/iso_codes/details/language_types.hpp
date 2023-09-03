/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include "fea/macros/enum_to_strings.hpp"

#include <cstdint>
#include <limits>
#include <string_view>

namespace fea {
enum class iso_639_scope : uint8_t {
	individual,
	macrolanguage,
	special,
	count,
};

enum class iso_639_type : uint8_t {
	ancient,
	constructed,
	extinct,
	historical,
	living,
	special,
	count,
};

enum class iso_639_status : uint8_t {
	active,
	deprecated,
	count,
};

enum class iso_639_deprecation_reason : uint8_t {
	change,
	duplicate,
	non_existent,
	split,
	merge,
	count,
};

// Runtime id type.
using iso_639_id_t = uint16_t;

// ISO 639 language description.
struct iso_639_lang {
	// The language scope.
	iso_639_scope scope{ iso_639_scope::count };

	// The language type.
	iso_639_type type{ iso_639_type::count };

	// The language status.
	iso_639_status status{ iso_639_status::count };

	// The reason for deprecation, if applicable.
	iso_639_deprecation_reason deprecation_reason{
		iso_639_deprecation_reason::count
	};

	// The runtime id of this language, for faster access.
	// Never, ever, serialize this id.
	iso_639_id_t runtime_id = (std::numeric_limits<uint16_t>::max)();

	// Runtime id of parent macro-language,
	// if applicable. Default initialized id otherwise.
	iso_639_id_t parent_macro_id = (std::numeric_limits<uint16_t>::max)();

	// Codes

	// ISO 639-3 code.
	std::string_view code_3{};

	// ISO 639-2B code.
	std::string_view code_2b{};

	// ISO 639-2T code.
	std::string_view code_2t{};

	// ISO 639-1 code.
	std::string_view code_1{};

	// Names

	// Language name (english).
	std::string_view name{};

	// Inverted language name (english).
	std::string_view inverted_name{};

	// TODO : native_name

	// Extra

	// ISO 639-3 code of parent macro language,
	// if applicable. Empty otherwise.
	std::string_view parent_macro_code_3{};

	// The 'changed to' code of a deprecated language,
	// if applicable. Empty otherwise.
	std::string_view changed_to{};
};
} // namespace fea

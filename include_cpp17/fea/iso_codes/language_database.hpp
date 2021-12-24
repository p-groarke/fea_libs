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
#include "fea/iso_codes/language_types.hpp"

#include <array>

namespace fea {
// WIP

// Indexed at runtime_id.
inline constexpr auto langs = std::array{
	iso_639_lang{
			iso_639_scope::count,
			iso_639_type::count,
			iso_639_status::count,
			iso_639_deprecation_reason::count,
			{},
			{},
			"bla",
			"bla",
			"bla",
			"bla",
			"bla",
			"bla",
			"bla",
			"bla",
	},
};
inline constexpr auto id3_to_rt = std::array{ iso_639_id_t{} };
inline constexpr auto id2t_to_rt = std::array{ iso_639_id_t{} };
inline constexpr auto id2b_to_rt = std::array{ iso_639_id_t{} };
inline constexpr auto id1_to_rt = std::array{ iso_639_id_t{} };

} // namespace fea

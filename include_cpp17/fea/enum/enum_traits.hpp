/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/enum/enum_array.hpp"
#include "fea/meta/non_type_traits.hpp"
#include "fea/meta/traits.hpp"

#include <array>
#include <limits>
#include <type_traits>

/*
fea::explode_enum calls your lambda with a variadic pack of
std::integral_constant<your_enum, val>...

*/

namespace fea {
// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// You must "extract" the non-type enum as it is passed by
// Passes std::integral_constant...
template <class Enum, size_t N, class Func>
constexpr auto explode_enum(const Func& func);

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// Overload for enums that contain a 'count' member.
// Passes std::integral_constant...
template <class Enum, class Func>
constexpr auto explode_enum(const Func& func);

// Calls your function with each non-type enum values.
// Enum must be from 0 to N.
// Provide N if your enum doesn't have the member 'count'.
// Passes std::integral_constant.
template <class Enum, size_t N = size_t(Enum::count), class Func>
constexpr void enum_for_each(const Func& func);

// Calls your function with each non-type enum values.
// Passes std::integral_constant.
template <auto... Args, class Func>
constexpr void enum_for_each(const Func& func);

// Creates a lookup array of size max enum + 1.
// Dereference the lookup with an enum value, to get its index in the variadic
// pack.
// Effectively, enables creation of programmatic switch-case lookups.
template <auto... Args>
constexpr auto make_enum_lookup();

} // namespace fea

#include "imp/enum_traits.imp.hpp"

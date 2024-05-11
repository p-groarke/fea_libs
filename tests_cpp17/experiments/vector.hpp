/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
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
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"

#include <type_traits>

/*
The fea::vector traits enable basic mathematical operations on your vector
types.

To support these operations, your vector type must specialize std::tuple_size,
std::tuple_element and provide a namespace::get<size_t> function.

Once that is added, register your vector type in the fea namespace by
specializing register_math_vector. Or you may use the helper macro.
*/

namespace fea {
template <class>
struct register_math_vector : std::false_type {};

template <class T>
inline constexpr bool is_math_vector_v = register_math_vector<T>::value;
} // namespace fea

// Helper to register (specialize) your class as a math vector.
// Use in namespace fea.
#define FEA_REGISTER_MATH_VECTOR(e) \
	template <> \
	struct register_math_vector<e> : std::true_type {}


// Implements mathematical operations on class.

template <class T>
constexpr std::enable_if_t<fea::is_math_vector_v<T>, T> operator+(
		const T& lhs, const T& rhs);

#include "imp/vector.imp.hpp"

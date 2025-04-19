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
#include <type_traits>

/*
fea::defensive : Various traits to enforce expected class properties.
*/

namespace fea {
// Makes sure 5 constructors/operators are present (destructor, copy
// constructor, move constructor, copy assignement constructor, move assignement
// constructor). Useful when using rule of 0 or when using = default.
template <class T>
constexpr bool fulfills_rule_of_5();

// Ensures your class is optimized to be stored in a vector. Checks whether
// it is trivially destructible (skips destructor call on resize) and
// trivially copyable (use memcpy on resive). If not, falls back
// to ensure your class is noexcept move constructible (vector resize cannot
// use your move constructor if it isn't marked noexcept).
template <class T>
constexpr bool fulfills_fast_vector();

// Ensures your class is move only. There is no copy constructor and no copy
// assignement operator. The move constructor and move assignement operator is
// present.
template <class T>
constexpr bool fulfills_move_only();

// Ensures your class is non constructible, aka has no default constructor,
// destructor, copy constructor, move constructor, copy assignement operator and
// move assignement operator. Useful when writing static singleton classes.
template <class T>
constexpr bool fulfills_non_constructible();
} // namespace fea


// Implementation
namespace fea {
template <class T>
constexpr bool fulfills_rule_of_5() {
	return std::is_destructible_v<T> && std::is_copy_constructible_v<T>
		&& std::is_copy_assignable_v<T> && std::is_move_constructible_v<T>
		&& std::is_move_assignable_v<T>;
}

template <class T>
constexpr bool fulfills_fast_vector() {
	return std::is_trivially_destructible_v<T>
		&& (std::is_trivially_copyable_v<T>
				|| std::is_nothrow_move_constructible_v<T>);
}

template <class T>
constexpr bool fulfills_move_only() {
	return std::is_move_constructible_v<T> && std::is_move_assignable_v<T>
		&& !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>;
}

template <class T>
constexpr bool fulfills_non_constructible() {
	return !std::is_default_constructible_v<T> && !std::is_destructible_v<T>
		&& !std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>
		&& !std::is_move_constructible_v<T> && !std::is_move_assignable_v<T>;
}
} // namespace fea

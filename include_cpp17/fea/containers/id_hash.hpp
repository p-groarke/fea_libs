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

namespace fea {
// Default id hasher.
// Specialize this struct for your own id classes.
// Return an unsigned type.
//
// The type of the return value will define how large the storage is inside some
// maps. Prefer returning precise unsigned types rather than always size_t.
template <class Key>
struct id_hash {
	constexpr Key operator()(const Key& k) const noexcept {
		return k;
	}
};

// Used internally to deduce the id_hash return type.
namespace detail {
template <class T>
struct id_hash_traits {
	using type
			= std::decay_t<decltype(std::declval<fea::id_hash<T>>().operator()(
					std::declval<T>()))>;
};

// Used internally to deduce the id_hash return type.
template <class T>
using id_hash_return_t = typename id_hash_traits<T>::type;
} // namespace detail
} // namespace fea

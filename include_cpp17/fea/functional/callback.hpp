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
#include "fea/meta/function_traits.hpp"
#include "fea/utils/platform.hpp"

/*
fea::callback enforces callback signatures on templated lambda callbacks.

It uses the most efficient callback mechanism, can be used / exposed in your API
and is strict (more strict than std::function).

First template argument is the function type (should come from template).
Second is the signature type, in the style of std::function.
For ex, fea::callback<FuncT, void(int)>
*/

namespace fea {
// Stores a callback function.
// First template argument is the function type (should come from template).
// Second is the signature type, as you would a std::function.
// For example, fea::callback<FuncT, void(int, float)>
template <class, class = void>
struct callback;

// Create a callback object.
// Provide a lambda that matches the callback signature.
template <class Func>
callback<Func> make_callback(const Func& func);

// Create a callback object.
// Provide a lambda that matches the callback signature.
template <class Func>
callback<Func> make_callback(Func&& func);

} // namespace fea

#include "imp/callback.imp.hpp"

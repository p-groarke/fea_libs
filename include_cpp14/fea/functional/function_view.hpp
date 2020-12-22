/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
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
#include <utility>

/*
function_view is a non-owning, fast std::function replacement.

It can wrap lambdas with closures or functors, but must be created with a
callable (so it doesn't survive the callable's lifetime). For example,

fea::function_view<void()> fn = [temp = 42](){ do_something(temp); };

More information :
https://vittorioromeo.info/index/blog/passing_functions_to_functions.html#%60function_view%60
*/

namespace fea {
template <class>
class function_view;

template <class FuncRet, class... FuncArgs>
struct function_view<FuncRet(FuncArgs...)> {
	template <class T>
	function_view(T&& t) noexcept
			: _ptr((void*)std::addressof(t))
			, _erased_func(do_erased<T>) {

#if FEA_CPP17
		static_assert(std::is_invocable_v<T&(FuncArgs...)>,
				"fea::function_view : signature mismatch with provided "
				"callable");
#endif

		static_assert(!std::is_same<std::decay_t<T>, function_view>::value,
				"fea::function_view : cannot pass in another function view");
	}

	FuncRet operator()(FuncArgs... func_args) const {
		return _erased_func(_ptr, std::forward<FuncArgs>(func_args)...);
	}

private:
	template <class T>
	constexpr static FuncRet do_erased(void* ptr, FuncArgs... func_args) {
		using ptr_t = typename std::add_pointer<T>::type;
		return (*reinterpret_cast<ptr_t>(ptr))(
				std::forward<FuncArgs>(func_args)...);
	}

	using erased_t = FuncRet (&)(void*, FuncArgs...);

	void* _ptr = nullptr;
	erased_t _erased_func;
};
} // namespace fea

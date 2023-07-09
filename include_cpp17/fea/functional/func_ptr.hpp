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
#include <functional>
#include <type_traits>

/*
func_ptr is a light-weight abstraction to help with constexpr function pointers.

It allows capturing either C function pointers or member function pointers at
compile time. This is required since std::function isn't constexpr. If it were,
func_ptr would be unnecessary.

For ex,
constexpr fea::func_ptr<void(obj*, int)> = &c_func; // ok
constexpr fea::func_ptr<void(obj*, int)> = &obj::func; // also ok
*/


namespace fea {
template <class>
struct func_ptr;

// Specialization for no arg function.
// Cannot be member function, since first argument would
// need to be object pointer.
template <class Ret>
struct func_ptr<Ret()> {
	using c_sig_t = Ret (*)();

	constexpr func_ptr() = default;
	constexpr func_ptr(c_sig_t ptr)
			: _c_ptr(ptr) {
	}

	std::function<Ret()> to_function() const {
		return { _c_ptr };
	}

private:
	c_sig_t _c_ptr = nullptr;
};


template <class Ret, class FrontT, class... Args>
struct func_ptr<Ret(FrontT, Args...)> {
	using c_sig_t = Ret (*)(FrontT, Args...);

	// This is the switcharoo to convert a signature in the form
	// 'void(const obj*, ...)' to 'void(obj*, ...) const'.
	// This is how std::function wants it, and how the reference will be
	// captured when using '&obj::func'.
	using nonconst_mem_sig_t = Ret (std::remove_pointer_t<FrontT>::*)(Args...);
	using const_mem_sig_t
			= Ret (std::remove_const_t<std::remove_pointer_t<FrontT>>::*)(
					Args...) const;
	using mem_sig_t
			= std::conditional_t<std::is_const_v<std::remove_pointer_t<FrontT>>,
					const_mem_sig_t, nonconst_mem_sig_t>;

	constexpr func_ptr() = default;
	constexpr func_ptr(c_sig_t ptr)
			: _c_ptr(ptr) {
	}
	constexpr func_ptr(mem_sig_t ptr)
			: _mem_ptr(ptr) {
	}

	// Does this func_ptr store a pointer?
	constexpr explicit operator bool() const noexcept {
		return _c_ptr != nullptr || _mem_ptr != nullptr;
	}

	// Invoke the function, passing its arguments.
	constexpr Ret invoke(FrontT arg1, Args... args) const {
		if (_c_ptr != nullptr) {
			return std::invoke(_c_ptr, std::forward<FrontT>(arg1),
					std::forward<Args>(args)...);
		}
		return std::invoke(_mem_ptr, std::forward<FrontT>(arg1),
				std::forward<Args>(args)...);
	}

	// Converts to a callable std::function.
	std::function<Ret(FrontT, Args...)> to_function() const {
		if (_c_ptr != nullptr) {
			return { _c_ptr };
		}
		return { _mem_ptr };
	}

private:
	c_sig_t _c_ptr = nullptr;
	mem_sig_t _mem_ptr = nullptr;
};

} // namespace fea

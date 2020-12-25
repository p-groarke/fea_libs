/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"

#include <cassert>
#include <functional>
#include <variant>

/*
fea::function_cl is a holder for function pointers, which does the "right thing"
when storing member pointers. It does *not* support captures or functors.

When storing member functions, the function signature must have an object
*pointer* as first parameter.

You may define 'FEA_BE_NAUGHTY' if you want faster performance and can live with
some undefined behavior. It will reinterpret_cast const member functions to
non-const, which works on MSVC at minimum.

fea::function is a trait which helps you choose between a std::function or
fea::function_cl.

By default, it returns a std::function type. Pass true as a second template
argument to use raw function pointers.
*/

namespace fea {
namespace detail {
template <bool, class>
struct function_cl;

// In this specialization, we are guaranteed to never hold member functions.
template <class FuncRet, class... FuncArgs>
struct function_cl<false, FuncRet(FuncArgs...)> {
	using plain_t = FuncRet (*)(FuncArgs...);

	// Can be default constructed.
	constexpr function_cl() = default;

	// Accept raw function pointers and captureless lambdas.
	constexpr function_cl(plain_t func)
			: _c_func(func) {
	}

	// Delete this constructor.
	// Remember the first argument of a member signature must be a pointer to
	// the class.
	template <class T, class... Args>
	constexpr function_cl(FuncRet (T::*)(Args...)) = delete;

	constexpr FuncRet operator()(FuncArgs... func_args) const {
		return _c_func(std::forward<FuncArgs>(func_args)...);
	}

private:
	plain_t _c_func = nullptr;
};

// We may, but are not guaranteed, store a member function pointer.
template <class FuncRet, class T, class... FuncArgs>
struct function_cl<true, FuncRet(T*, FuncArgs...)> {
	using plain_t = FuncRet (*)(T*, FuncArgs...);
	using member_t = FuncRet (T::*)(FuncArgs...);
	using const_member_t = FuncRet (T::*)(FuncArgs...) const;

	// Can be default constructed.
	constexpr function_cl() = default;

	// Accept raw function pointers and captureless lambdas.
	constexpr function_cl(plain_t func) noexcept
			: _func_union(func) {
	}

	// Accept member pointers.
	constexpr function_cl(member_t func) noexcept
			: _func_union(func)
			, _held_func(held_func::member) {
	}

	// Accept const member pointers.
	constexpr function_cl(const_member_t func) noexcept
			: _func_union(func)
			, _held_func(held_func::const_member) {
	}

	constexpr FuncRet operator()(T* obj, FuncArgs... func_args) const {
		switch (_held_func) {
		case held_func::plain: {
			return _func_union.plain_func(
					obj, std::forward<FuncArgs>(func_args)...);
		} break;
		case held_func::member: {
			return (obj->*_func_union.mem_func)(
					std::forward<FuncArgs>(func_args)...);
		} break;
		case held_func::const_member: {
			return (obj->*_func_union.const_mem_func)(
					std::forward<FuncArgs>(func_args)...);
		} break;
		default: {
			assert(false);
		} break;
		}
	}

	explicit operator bool() const noexcept {
		switch (_held_func) {
		case held_func::plain: {
			return _func_union.c_func != nullptr;
		} break;
		case held_func::member: {
			return _func_union.mem_func != nullptr;
		} break;
		case held_func::const_member: {
			return _func_union.const_mem_func != nullptr;
		} break;
		default: {
			assert(false);
		} break;
		}
	}

private:
	// constexpr FuncRet do_plain(T* obj, FuncArgs... args) const {
	//	return _func_union.c_func(obj, std::forward<FuncArgs>(args)...);
	//}
	// constexpr FuncRet do_member(T* obj, FuncArgs... args) const {
	//	return (obj->*_func_union.mem_func)(std::forward<FuncArgs>(args)...);
	//}
	// constexpr FuncRet do_const_member(T* obj, FuncArgs... args) const {
	//	return (obj->*_func_union.const_mem_func)(
	//			std::forward<FuncArgs>(args)...);
	//}

	// friend auto operator->*(T& obj, FuncRet (*f)(T*, FuncArgs...)) {
	//	return [o = &obj, f_ptr = f](FuncArgs... args) {
	//		f_ptr(o, std::forward<decltype(FuncArgs)>(args)...);
	//	};
	//}

	// friend auto operator->*(T& obj, FuncRet (T::*f)(FuncArgs...)) {
	//	return [o = &obj, f_ptr = f](FuncArgs... args) {
	//		(o->*f_ptr)(std::forward<decltype(FuncArgs)>(args)...);
	//	};
	//}

	// friend auto operator->*(T& obj, FuncRet (T::*f)(FuncArgs...) const) {
	//	return [o = &obj, f_ptr = f](FuncArgs... args) {
	//		(o->*f_ptr)(std::forward<decltype(FuncArgs)>(args)...);
	//	};
	//}

	union func_u {
		constexpr explicit func_u(plain_t f) noexcept
				: plain_func(f) {
		}
		constexpr explicit func_u(member_t f) noexcept
				: mem_func(f) {
		}
		constexpr explicit func_u(const_member_t f) noexcept
				: const_mem_func(f) {
		}

		plain_t plain_func;
		member_t mem_func;
		const_member_t const_mem_func;
	};

	enum class held_func : uint8_t {
		plain,
		member,
		const_member,
		// count,
	};

	func_u _func_union{ plain_t(nullptr) };
	held_func _held_func = held_func::plain;
};

// Selects the appropriate class by checking if first signature argument is a
// pointer to a class.
template <class>
struct function_cl_selector;

template <class Ret, class... Args>
struct function_cl_selector<Ret(Args...)> {
	using type = detail::function_cl<false, Ret(Args...)>;
};

template <class Ret, class T, class... Args>
struct function_cl_selector<Ret(T*, Args...)> {
	using type = detail::function_cl<std::is_class<T>::value, Ret(T*, Args...)>;
};

} // namespace detail


// A wrapper on function pointers.
// Doesn't support state (captures).
// Much faster than std::function for its purpose.
template <class... Args>
using function_cl = typename detail::function_cl_selector<Args...>::type;

// template <class... Args>
// using function_cl = function_cl<Args...>;


namespace detail {
template <class, bool = false>
struct function;

template <class Func>
struct function<Func, false> {
	using type = std::function<Func>;
};

template <class Func>
struct function<Func, true> {
	using type = fea::function_cl<Func>;
};
} // namespace detail

// Selects either std::function or fea::function_cl with the bool.
// fea::function_cl is a stateless and capture light-weight wrapper than can
// store function pointers and member function pointers.
template <class Func, bool UsePtr = false>
using function = typename detail::function<Func, UsePtr>::type;


} // namespace fea

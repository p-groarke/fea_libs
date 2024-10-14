#if FEA_LINUX
// GCC flags partly out of bounds on member function deref.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

namespace fea {
namespace detail {
template <class R, class... A>
constexpr function_cl<false, R(A...)>::function_cl(plain_t func) noexcept
		: _plain_func(func) {
}

template <class R, class... A>
template <class L>
constexpr function_cl<false, R(A...)>::function_cl(L&& l) noexcept
		: _plain_func(plain_t(std::forward<L>(l))) {

	static_assert(std::is_convertible_v<L, plain_t>,
			"fea::function_cl : Cannot convert from L to function pointer. "
			"Did you pass in a lambda with a capture?");
}

template <class R, class... A>
constexpr R function_cl<false, R(A...)>::operator()(A... func_args) const {
	return _plain_func(std::forward<A>(func_args)...);
}

template <class R, class... A>
constexpr function_cl<false, R(A...)>::operator bool() const noexcept {
	return _plain_func != nullptr;
}


template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::function_cl(plain_t func) noexcept
		: _func_union(func)
		, _held_func(held_func::plain) {
}

template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::function_cl(member_t func) noexcept
		: _func_union(func)
		, _held_func(held_func::member) {
}

template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::function_cl(
		const_member_t func) noexcept
		: _func_union(func)
		, _held_func(held_func::const_member) {
}

template <class R, class F, class... A>
template <class L>
constexpr function_cl<true, R(F*, A...)>::function_cl(L&& l) noexcept
		: _func_union(plain_t(std::forward<L>(l)))
		, _held_func(held_func::plain) {

	static_assert(std::is_convertible_v<L, plain_t>,
			"fea::function_cl : Cannot convert from L to function pointer. "
			"Did you pass in a lambda with a capture?");
}

template <class R, class F, class... A>
constexpr R function_cl<true, R(F*, A...)>::operator()(
		F* obj, A... func_args) const {
	switch (_held_func) {
	case held_func::plain: {
		return _func_union.plain_func(obj, std::forward<A>(func_args)...);
	} break;
	case held_func::member: {
		return (obj->*_func_union.mem_func)(std::forward<A>(func_args)...);
	} break;
	case held_func::const_member: {
		return (obj->*_func_union.const_mem_func)(
				std::forward<A>(func_args)...);
	} break;
	default: {
		assert(false);
		// Prevent compile error. This should never be executed.
		return _func_union.plain_func(obj, std::forward<A>(func_args)...);
	} break;
	}
}

template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::operator bool() const noexcept {
	switch (_held_func) {
	case held_func::plain: {
		return _func_union.plain_func != nullptr;
	} break;
	case held_func::member: {
		return _func_union.mem_func != nullptr;
	} break;
	case held_func::const_member: {
		return _func_union.const_mem_func != nullptr;
	} break;
	default: {
		return false;
	} break;
	}
}

template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::func_u::func_u() noexcept
		: plain_func(nullptr) {
}
template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::func_u::func_u(plain_t f) noexcept
		: plain_func(f) {
}
template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::func_u::func_u(member_t f) noexcept
		: mem_func(f) {
}
template <class R, class F, class... A>
constexpr function_cl<true, R(F*, A...)>::func_u::func_u(
		const_member_t f) noexcept
		: const_mem_func(f) {
}

template <class Ret, class... Args>
struct function_cl_selector<Ret(Args...)> {
	using type = detail::function_cl<false, Ret(Args...)>;
};

template <class Ret, class T, class... Args>
struct function_cl_selector<Ret(T*, Args...)> {
	using type = detail::function_cl<std::is_class_v<T>, Ret(T*, Args...)>;
};
} // namespace detail
} // namespace fea

#if FEA_LINUX
#pragma GCC diagnostic pop
#endif

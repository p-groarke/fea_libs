namespace fea {

// Final inheritance stage.
// Store the callback function or function object.
template <class Func, class Ret, class... Args>
struct callback<Func, Ret(Args...)> {
	using func_t = Func;

	constexpr callback(const Func& func)
			: _func(func) {
	}
	constexpr callback(Func&& func)
			: _func(std::move(func)) {
	}

	constexpr Ret operator()(Args... args) const {
		return _func(std::forward<Args>(args)...);
	}

private:
	Func _func;
};

// Second inheritance stage.
// Deduce the appropriate signature.
namespace detail {
template <class, class>
struct callback_base;

template <class Func, class Ret, class... Args>
struct callback_base<Func, Ret(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class... Args>
struct callback_base<Func, Ret (*)(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class T, class... Args>
struct callback_base<Func, Ret (T::*)(Args...)> : callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class Func, class Ret, class T, class... Args>
struct callback_base<Func, Ret (T::*)(Args...) const>
		: callback<Func, Ret(Args...)> {
	using base = callback<Func, Ret(Args...)>;
	using base::base;
};

template <class, class = void>
struct callback_selector;


template <class Func>
struct callback_selector<Func,
		std::enable_if_t<fea::is_detected_v<fea::has_operator_paren, Func>>>
		: callback_base<Func, decltype(&Func::operator())> {
	using base = callback_base<Func, decltype(&Func::operator())>;
	using base::base;
};

template <class Func>
struct callback_selector<Func,
		std::enable_if_t<!fea::is_detected_v<fea::has_operator_paren, Func>>>
		: callback_base<Func, Func> {
	using base = callback_base<Func, Func>;
	using base::base;
};

} // namespace detail

// First stage (child class).
// Specialization preferred by compiler.
// It will recursively inherit other classes to end up with a type that has the
// signature type in the second template argument (the lambda type is in the
// first template arg).
template <class Func, class>
struct callback : detail::callback_selector<Func> {
	using func_t = Func;
	using base = detail::callback_selector<Func>;

	constexpr callback(const Func& func)
			: base(func) {
	}
	constexpr callback(Func&& func)
			: base(std::move(func)) {
	}
};


template <class Func>
callback<Func> make_callback(const Func& func) {
	return callback<Func>(func);
}

template <class Func>
callback<Func> make_callback(Func&& func) {
	return callback<Func>(func);
}

} // namespace fea

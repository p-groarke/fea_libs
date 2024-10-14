namespace fea {
// Non-member implementation.

template <class R, class... A>
constexpr func_ptr<R(A...)>::func_ptr(c_sig_t ptr)
		: _c_ptr(ptr) {
}

template <class R, class... A>
constexpr func_ptr<R(A...)>::operator bool() const noexcept {
	return _c_ptr != nullptr;
}

template <class R, class... A>
constexpr R func_ptr<R(A...)>::invoke(A... args) const {
	return (*_c_ptr)(std::forward<A>(args)...);
}

template <class R, class... A>
constexpr R func_ptr<R(A...)>::operator()(A... args) const {
	return invoke(std::forward<A>(args)...);
}

template <class R, class... A>
std::function<R(A...)> func_ptr<R(A...)>::to_function() const {
	return { _c_ptr };
}


// Member implementation.

template <class R, class F, class... A>
constexpr func_ptr<R(F*, A...)>::func_ptr(c_sig_t ptr)
		: _c_ptr(ptr) {
}

template <class R, class F, class... A>
constexpr func_ptr<R(F*, A...)>::func_ptr(mem_sig_t ptr)
		: _mem_ptr(ptr) {
}

template <class R, class F, class... A>
constexpr func_ptr<R(F*, A...)>::operator bool() const noexcept {
	return _c_ptr != nullptr || _mem_ptr != nullptr;
}

template <class R, class F, class... A>
constexpr R func_ptr<R(F*, A...)>::invoke(F* arg1, A... args) const {
	if (_c_ptr != nullptr) {
		return std::invoke(
				_c_ptr, std::forward<F*>(arg1), std::forward<A>(args)...);
	}
	return std::invoke(
			_mem_ptr, std::forward<F*>(arg1), std::forward<A>(args)...);
}

template <class R, class F, class... A>
constexpr R func_ptr<R(F*, A...)>::operator()(F* arg1, A... args) const {
	return invoke(std::forward<F*>(arg1), std::forward<A>(args)...);
}

template <class R, class F, class... A>
std::function<R(F*, A...)> func_ptr<R(F*, A...)>::to_function() const {
	if (_c_ptr != nullptr) {
		return { _c_ptr };
	}
	return { _mem_ptr };
}

} // namespace fea

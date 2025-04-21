namespace fea {
template <class Func>
on_exit<Func>::on_exit(const Func& func)
		: _func(func) {
}

template <class Func>
on_exit<Func>::on_exit(Func&& func)
		: _func(std::move(func)) {
}

template <class Func>
on_exit<Func>::~on_exit() {
	_func();
}

template <class Func>
auto make_on_exit(Func&& func) {
	return on_exit<Func>{ std::forward<Func>(func) };
}

} // namespace fea

namespace fea {
template <class T>
back_ptr<T>::back_ptr(T* ptr) noexcept
		: _ptr(ptr) {
}

template <class T>
back_ptr<T>::back_ptr(const back_ptr&) noexcept
		: _ptr(nullptr) {
}

template <class T>
back_ptr<T>::back_ptr(back_ptr&& other) noexcept
		: _ptr(nullptr) {
	other._ptr = nullptr;
}

template <class T>
auto back_ptr<T>::operator=(const back_ptr& other) noexcept -> back_ptr& {
	if (this != &other) {
		_ptr = nullptr;
	}
	return *this;
}

template <class T>
auto back_ptr<T>::operator=(back_ptr&& other) noexcept -> back_ptr& {
	if (this != &other) {
		_ptr = nullptr;
		other._ptr = nullptr;
	}
	return *this;
}

template <class T>
auto back_ptr<T>::operator=(T* ptr) noexcept -> back_ptr& {
	_ptr = ptr;
	return *this;
}

template <class T>
T& back_ptr<T>::operator*() const noexcept {
	return *_ptr;
}

template <class T>
T* back_ptr<T>::operator->() const noexcept {
	return _ptr;
}

template <class T>
T* back_ptr<T>::get() const noexcept {
	return _ptr;
}

template <class T>
void back_ptr<T>::reset(T* ptr /*= nullptr*/) noexcept {
	_ptr = ptr;
}

template <class T>
back_ptr<T>::operator bool() const noexcept {
	return _ptr != nullptr;
}

} // namespace fea

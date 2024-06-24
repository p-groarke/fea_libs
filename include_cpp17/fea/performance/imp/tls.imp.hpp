namespace fea {
template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls<T, Alloc>& storage)
		: tls_lock(std::move(storage.lock())) {
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls_lock&& other) noexcept
		: _tid(other._tid)
		, _idx(other._idx)
		, _value(other._value)
		, _storage(other._storage) {
	other._tid = std_thread_id_t{};
	other._idx = (std::numeric_limits<uint32_t>::max)();
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(std_thread_id_t tid, size_type idx, T& value,
		tls<T, Alloc>& storage) noexcept
		: _tid(tid)
		, _idx(idx)
		, _value(value)
		, _storage(storage) {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::~tls_lock() {
	if (_tid != std_thread_id_t{}) {
		assert(_tid == std::this_thread::get_id());
		assert(_idx != (std::numeric_limits<uint32_t>::max)());
		_storage.unlock(_tid, _idx);
	}
}


template <class T, class Alloc>
const T& tls_lock<T, Alloc>::local() const& {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
T& tls_lock<T, Alloc>::local() & {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
tls<T, Alloc>::~tls() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Destroying storage with unreleased locks. Make sure "
					"all your threads are done working before destroying "
					"storage.");
		}
	}
}

template <class T, class Alloc>
tls_lock<T, Alloc> tls<T, Alloc>::lock() {
	std_thread_id_t tid = std::this_thread::get_id();

	// Optimization.
	// We are guaranteed atomic for this thread id.
	// That is, the same thread cannot re-enter this function (or any other),
	// so we do not have a race condition for unlocking.
	//
	// Also, std::deque doesn't invalidate pre-existing iterators.
	//
	// This means we can search for a free slot for this thread without locking.
	// However, we use our "valid" size instead of the deque size,
	// in case it is being modified.
	{
		size_type size = _locks.size();
		auto begin_it = _locks.begin();

		size_type i = 0;
		for (; i < size; ++i) {
			const thread_info& info = *begin_it;
			if (info.thread_id == tid && info.locked == false) {
				break;
			}
			++begin_it;
		}

		// auto end_it = std::next(begin_it, size);

		// auto it = std::find_if(begin_it, end_it, [&](const thread_info& info)
		// { 	return info.thread_id == tid && info.locked == false;
		// });

		// End iterator could have changed, compare with our cached size.
		// if (idx != size) {
		if (i < size) {
			// Found free thread slot.
			thread_info& info = *begin_it;
			assert(!info.locked);

			info.locked = true;
			return tls_lock<T, Alloc>{
				tid,
				uint32_t(i),
				*std::next(_datas.begin(), i),
				*this,
			};
		}
	}

	// If we reach here, we must initialize data and modify the locks deque.
	// We lock.
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	// Create new lock + data slot, then update valid size.
	size_t idx = _locks.size();
	_datas.push_back({});
	_locks.push_back(thread_info{ tid, true });
	assert(_locks.size() == _datas.size());

	return tls_lock<T, Alloc>{
		tid,
		idx,
		*std::next(_datas.begin(), idx),
		*this,
	};
}

template <class T, class Alloc>
void tls<T, Alloc>::unlock(
		[[maybe_unused]] std_thread_id_t tid, size_type idx) {
	// Since the lock should have been created already, no need for locking.
	// Just use our cached valid size.
	size_type size = _locks.size();
	if (idx >= size) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that doesn't exist.");
	}

	thread_info& info = *std::next(_locks.begin(), idx);
	assert(info.thread_id == tid);

	// thread_info& info = _locks[idx];
	if (!info.locked) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that isn't locked.");
	}

	// Unlock it.
	info.locked = false;
}

template <class T, class Alloc>
bool tls<T, Alloc>::empty() const {
	assert(_locks.size() == _datas.size());
	return _locks.empty();
}

template <class T, class Alloc>
auto tls<T, Alloc>::size() const -> size_type {
	assert(_locks.size() == _datas.size());
	return _locks.size();
}

template <class T, class Alloc>
void tls<T, Alloc>::clear() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot clear storage, at least 1 thread currently "
					"owns a lock on storage.");
		}
	}

	_datas.clear();
	_locks.clear();
	assert(_locks.size() == _datas.size());
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) const {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot combine storage, at least 1 thread still holds "
					"a lock.");
		}
	}

	for (const T& t : _datas) {
		func(t);
	}
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot combine storage, at least 1 thread still holds "
					"a lock.");
		}
	}

	for (T& t : _datas) {
		func(t);
	}
}
} // namespace fea

namespace fea {
template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls<T, Alloc>& storage)
		: tls_lock(std::move(storage.lock())) {
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls_lock&& other) noexcept
		: _tid(other._tid)
		, _data_idx(other._data_idx)
		, _value(other._value)
		, _storage(other._storage) {
	other._tid = std_thread_id_t{};
	other._data_idx = (std::numeric_limits<uint32_t>::max)();
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(std_thread_id_t tid, uint32_t data_idx, T& value,
		tls<T, Alloc>& storage) noexcept
		: _tid(tid)
		, _data_idx(data_idx)
		, _value(value)
		, _storage(storage) {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::~tls_lock() {
	if (_tid != std_thread_id_t{}) {
		assert(_tid == std::this_thread::get_id());
		assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
		_storage.unlock(_tid, _data_idx);
	}
}


template <class T, class Alloc>
const T& tls_lock<T, Alloc>::local() const& {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
T& tls_lock<T, Alloc>::local() & {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}


// template <class T, class Alloc>
// tls<T, Alloc>::tls() {
// }

// template <class T, class Alloc>
// void tls<T, Alloc>::init() {
//	std_thread_id_t tid = std::this_thread::get_id();
//
//	// We MUST initialize the locks deque to guarantee the begin
//	// iterator is always valid.
//	std::lock_guard<std::shared_mutex> g{ _mutex };
//	assert(_locks.size() == _valid_locks_size);
//	assert(_datas.size() == _valid_locks_size);
//
//	// Create new lock + data slot.
//	_datas.emplace_back();
//	_locks.push_back(thread_info{ tid, 0u, false });
//	_first_lock = &_locks.front();
//	_valid_locks_size = 1u;
//
//	assert(_locks.size() == _valid_locks_size);
//	assert(_datas.size() == _valid_locks_size);
//	assert(_first_lock != nullptr);
// }

template <class T, class Alloc>
tls<T, Alloc>::~tls() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

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
		size_type size = _valid_locks_size;
		auto begin_it = _locks.begin();
		auto end_it = std::next(begin_it, size);

		auto it = std::find_if(begin_it, end_it, [&](const thread_info& info) {
			return info.thread_id == tid && info.locked == false;
		});

		// size_type idx = size;
		// for (size_t i = 0; i < size; ++i) {
		//	const thread_info* ti = &_locks.at(i);
		//	if (ti->thread_id == tid && ti->locked == false) {
		//		idx = i;
		//		break;
		//	}
		// }

		// End iterator could have changed, compare with our cached size.
		// if (idx != size) {
		if (it != end_it) {
			// Found free thread slot.
			thread_info& info = *it;
			assert(info.idx < size);
			assert(!info.locked);
			info.locked = true;

			return tls_lock<T, Alloc>{
				tid,
				info.idx,
				_datas[info.idx],
				*this,
			};
		}
	}

	// If we reach here, we must initialize data and modify the locks deque.
	// We lock.
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

	// Create new lock + data slot, then update valid size.
	_locks.push_back(thread_info{ tid, uint32_t(_datas.size()), true });
	_datas.emplace_back();
	_valid_locks_size = _locks.size();
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

	// Only 1 lock points to data.
	assert(std::count_if(_locks.begin(), _locks.end(),
				   [&, this](const thread_info& ti) {
					   return ti.idx == _locks.back().idx;
				   })
			== ptrdiff_t(1));

	return tls_lock<T, Alloc>{
		tid,
		_locks.back().idx,
		_datas[_locks.back().idx],
		*this,
	};
}

template <class T, class Alloc>
void tls<T, Alloc>::unlock(std_thread_id_t tid, uint32_t idx) {
	// Since the lock should have been created already, no need for locking.
	// Just use our cached valid size.
	size_type size = _valid_locks_size;
	if (idx >= size) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that doesn't exist.");
	}

	thread_info& info = *std::next(_locks.begin(), idx);
	assert(info.thread_id == tid);
	assert(info.idx == idx);

	// size_type idx = size;

	// for (size_t i = 0; i < size; ++i) {
	//	const thread_info& ti = _locks[i];
	//	if (ti.thread_id == tid && ti.data_idx == data_idx) {
	//		idx = i;
	//		break;
	//	}
	// }

	// if (idx == size) {
	//	fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
	//			"Trying to unlock tls that doesn't exist.");
	// }

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
	return _valid_locks_size == 0u;
}

template <class T, class Alloc>
auto tls<T, Alloc>::size() const -> size_type {
	return _valid_locks_size;
}

template <class T, class Alloc>
void tls<T, Alloc>::clear() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot clear storage, at least 1 thread currently "
					"owns a lock on storage.");
		}
	}

	_locks.clear();
	_datas.clear();
	_valid_locks_size = 0u;
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) const {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

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
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

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

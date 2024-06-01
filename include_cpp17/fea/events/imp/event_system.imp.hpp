namespace fea {
template <class, class, class...>
struct event_system;

// could be : using notifier_id = size_t;
struct notifier_id {
	notifier_id() = default;
	~notifier_id() = default;
	notifier_id(const notifier_id&) = default;
	notifier_id(notifier_id&&) = default;
	notifier_id& operator=(const notifier_id&) = default;
	notifier_id& operator=(notifier_id&&) = default;

private:
	template <class, class, class...>
	friend struct event_system;

	inline notifier_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};

// Channel callback Id
template <class EventEnum, EventEnum e, class ChannelEnum, ChannelEnum c>
struct event_sys_id {
	event_sys_id() = default;

private:
	template <class, class, class...>
	friend struct event_system;

	event_sys_id(event_id<EventEnum, e> eid)
			: _eid(eid) {
	}

	event_id<EventEnum, e> _eid;
};


// Notifier callback Id
template <class EventEnum, EventEnum e>
struct event_sys_id<EventEnum, e> {
	event_sys_id() = default;

	notifier_id nid() const {
		return _nid;
	}

private:
	template <class, class, class...>
	friend struct event_system;

	event_sys_id(notifier_id nid_, event_id<EventEnum, e> eid)
			: _nid(nid_)
			, _eid(eid) {
	}

	notifier_id _nid;
	event_id<EventEnum, e> _eid;
};


template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		notifier_id nid) {
	return _notifier_stacks.contains(nid._id);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e> id) {
	if (!contains(id._nid)) {
		return false;
	}
	return _notifier_stacks.at_unchecked(id._nid._id).contains(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	return std::get<size_t(c)>(_channel_stacks).contains(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::contains(
		event_sys_id<EventEnum, e, ChannelEnum, ChannelEnum::count>) {
	return false;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e> id) const {
	return _notifier_stacks.at(id._nid._id).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e> id) {
	return _notifier_stacks.at(id._nid._id).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) const {
	return std::get<size_t(c)>(_channel_stacks).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	return std::get<size_t(c)>(_channel_stacks).at(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e> id) const {
	assert(contains(id));
	return _notifier_stacks.at_unchecked(id._nid._id).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e> id) {
	assert(contains(id));
	return _notifier_stacks.at_unchecked(id._nid._id).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
const auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) const {
	assert(contains(id));
	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
auto& event_system<EventEnum, ChannelEnum, FuncTypes...>::at_unchecked(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	assert(contains(id));
	return std::get<size_t(c)>(_channel_stacks).at_unchecked(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	for (const auto& stack_pair : _notifier_stacks) {
		if (!stack_pair.second.empty()) {
			return false;
		}
	}

	bool ret = true;
	tuple_for_each(
			[&](const auto& stack) { ret &= stack.empty(); }, _channel_stacks);
	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::event_empty()
		const noexcept {
	for (const auto& stack_pair : _notifier_stacks) {
		if (!stack_pair.second.template empty<e>()) {
			return false;
		}
	}

	bool ret = true;
	tuple_for_each([&](const auto& stack) { ret &= stack.template empty<e>(); },
			_channel_stacks);
	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).empty();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template empty<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	return std::get<size_t(c)>(_channel_stacks).empty();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
bool event_system<EventEnum, ChannelEnum, FuncTypes...>::empty()
		const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template empty<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	size_t ret = 0;
	for (const auto& stack_pair : _notifier_stacks) {
		ret += stack_pair.second.size();
	}

	tuple_for_each(
			[&](const auto& stack) { ret += stack.size(); }, _channel_stacks);

	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::event_size()
		const noexcept {
	size_t ret = 0;
	for (const auto& stack_pair : _notifier_stacks) {
		ret += stack_pair.second.template size<e>();
	}

	tuple_for_each([&](const auto& stack) { ret += stack.template size<e>(); },
			_channel_stacks);

	return ret;
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::size(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).size();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::size(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template size<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).size();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::size() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template size<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		notifier_id nid, size_t new_cap) {
	_notifier_stacks.at(nid._id).reserve(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		notifier_id nid, size_t new_cap) {
	_notifier_stacks.at(nid._id).template reserve<e>(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		size_t new_cap) {
	std::get<size_t(c)>(_channel_stacks).reserve(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::reserve(
		size_t new_cap) {
	std::get<size_t(c)>(_channel_stacks).template reserve<e>(new_cap);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
size_t event_system<EventEnum, ChannelEnum, FuncTypes...>::capacity(
		notifier_id nid) const noexcept {
	return _notifier_stacks.at(nid._id).template capacity<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
size_t
event_system<EventEnum, ChannelEnum, FuncTypes...>::capacity() const noexcept {
	return std::get<size_t(c)>(_channel_stacks).template capacity<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	_notifier_stacks.clear();
	tuple_for_each([](auto& stack) { stack.clear(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear_subscribers() {
	for (auto& stack_pair : _notifier_stacks) {
		stack_pair.second.clear();
	}
	tuple_for_each([](auto& stack) { stack.clear(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::event_clear() {
	for (auto& stack_pair : _notifier_stacks) {
		stack_pair.second.template clear<e>();
	}
	tuple_for_each(
			[](auto& stack) { stack.template clear<e>(); }, _channel_stacks);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear(
		notifier_id nid) {
	_notifier_stacks.at(nid._id).clear();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear(
		notifier_id nid) {
	_notifier_stacks.at(nid._id).template clear<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	std::get<size_t(c)>(_channel_stacks).clear();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::clear() {
	std::get<size_t(c)>(_channel_stacks).template clear<e>();
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
notifier_id event_system<EventEnum, ChannelEnum, FuncTypes...>::add_notifier() {
	assert(_notifier_id_generator != (std::numeric_limits<size_t>::max)());

	// 0 is never used, reserved for future.
	++_notifier_id_generator;

	_notifier_stacks.insert({ _notifier_id_generator, {} });
	return { _notifier_id_generator };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::remove_notifier(
		notifier_id nid) {
	_notifier_stacks.erase(nid._id);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class Func>
event_sys_id<EventEnum, e>
event_system<EventEnum, ChannelEnum, FuncTypes...>::subscribe(
		notifier_id nid, Func&& callback) {
	return { nid, _notifier_stacks.at(nid._id).template subscribe<e>(
						  std::forward<Func>(callback)) };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class Func>
event_sys_id<EventEnum, e, ChannelEnum, c>
event_system<EventEnum, ChannelEnum, FuncTypes...>::subscribe(Func&& func) {
	return { std::get<size_t(c)>(_channel_stacks)
					 .template subscribe<e>(std::forward<Func>(func)) };
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::unsubscribe(
		event_sys_id<EventEnum, e> id) {
	_notifier_stacks.at(id._nid._id).template unsubscribe<e>(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::unsubscribe(
		event_sys_id<EventEnum, e, ChannelEnum, c> id) {
	std::get<size_t(c)>(_channel_stacks).template unsubscribe<e>(id._eid);
}

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger(
		notifier_id nid, Args&&... args) {
	_notifier_stacks.at(nid._id).template trigger<e>(
			std::forward<Args>(args)...);
}

#if FEA_WITH_TBB
template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger_mt(
		notifier_id nid, Args&&... args) {
	_notifier_stacks.at(nid._id).template trigger_mt<e>(
			std::forward<Args>(args)...);
}
#endif

template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger(
		Args&&... args) {
	std::get<size_t(c)>(_channel_stacks)
			.template trigger<e>(std::forward<Args>(args)...);
}

#if FEA_WITH_TBB
template <class EventEnum, class ChannelEnum, class... FuncTypes>
template <ChannelEnum c, EventEnum e, class... Args>
void event_system<EventEnum, ChannelEnum, FuncTypes...>::trigger_mt(
		Args&&... args) {
	std::get<size_t(c)>(_channel_stacks)
			.template trigger_mt<e>(std::forward<Args>(args)...);
}
#endif

} // namespace fea

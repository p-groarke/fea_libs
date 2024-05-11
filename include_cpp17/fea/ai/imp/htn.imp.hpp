namespace fea {
namespace detail {
template <size_t N, class T>
bool has_duplicates(fea::span<const T> spn) {
	fea::stack_vector<T, N> cpy(spn.begin(), spn.end());
	std::sort(cpy.begin(), cpy.end());
	if (std::adjacent_find(cpy.begin(), cpy.end()) != cpy.end()) {
		return true;
	}
	return false;
}

template <class T>
bool has_duplicates(fea::span<const T> spn) {
	std::vector<T> cpy(spn.begin(), spn.end());
	std::sort(cpy.begin(), cpy.end());
	if (std::adjacent_find(cpy.begin(), cpy.end()) != cpy.end()) {
		return true;
	}
	return false;
}
} // namespace detail


template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::add_operator(OperatorEnum op) {
	assert(op != OperatorEnum::count);
	_operator = op;
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
template <class EffectFunc>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::add_effect(EffectFunc&& eff_func) {
	_effects = std::forward<EffectFunc>(eff_func);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
template <class EffectFunc>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::add_expected_effect(EffectFunc&& eff_func) {
	_expected_effects = std::forward<EffectFunc>(eff_func);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::add_predicates(fea::span<const PredicateEnum> preds) {
	if (preds.size() + _predicates.size() > _max_preds) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.insert(_predicates.end(), preds.begin(), preds.end());
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState, EffectArgs...>::
		add_predicates(std::initializer_list<PredicateEnum>&& preds) {
	add_predicates({ preds.begin(), preds.end() });
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::add_predicate(PredicateEnum pred) {
	if (_predicates.size() + 1 > _max_preds) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.push_back(pred);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::apply_effects_and_expected(WorldState& w,
		EffectArgs... args) const {
	_effects(&w, std::forward<EffectArgs>(args)...);

	if (_expected_effects) {
		_expected_effects(&w, std::forward<EffectArgs>(args)...);
	}
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::apply_effects(WorldState& w, EffectArgs... args) const {
	_effects(&w, std::forward<EffectArgs>(args)...);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
bool htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::has_effects() const {
	return bool(_effects);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
bool htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::has_expected_effects() const {
	return bool(_expected_effects);
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
bool htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::has_operator() const {
	return _operator != OperatorEnum::count;
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
fea::span<const PredicateEnum>
htn_action<PredicateEnum, OperatorEnum, WorldState, EffectArgs...>::predicates()
		const {
	return { _predicates.begin(), _predicates.end() };
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
OperatorEnum
htn_action<PredicateEnum, OperatorEnum, WorldState, EffectArgs...>::operator_e()
		const {
	return _operator;
}

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
void htn_action<PredicateEnum, OperatorEnum, WorldState,
		EffectArgs...>::validate() const {
	if (!has_effects()) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Action missing effect.");
	}

	if (!has_operator()) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Action missing operator.");
	}

	if (detail::has_duplicates<_max_preds>(predicates())) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Action predicates should not contain "
				"duplicates.");
	}
}


template <class TaskEnum, class ActionEnum>
constexpr htn_subtask<TaskEnum, ActionEnum>::htn_subtask(TaskEnum t)
		: _task(t) {
	assert(!is_action());
}

template <class TaskEnum, class ActionEnum>
constexpr htn_subtask<TaskEnum, ActionEnum>::htn_subtask(ActionEnum a)
		: _action(a) {
	assert(!is_task());
}

template <class TaskEnum, class ActionEnum>
constexpr bool htn_subtask<TaskEnum, ActionEnum>::is_task() const {
	return _task != TaskEnum::count;
}

template <class TaskEnum, class ActionEnum>
constexpr bool htn_subtask<TaskEnum, ActionEnum>::is_action() const {
	return _action != ActionEnum::count;
}

template <class TaskEnum, class ActionEnum>
constexpr TaskEnum htn_subtask<TaskEnum, ActionEnum>::task() const {
	return _task;
}

template <class TaskEnum, class ActionEnum>
constexpr ActionEnum htn_subtask<TaskEnum, ActionEnum>::action() const {
	return _action;
}


template <class T, class A>
constexpr bool operator==(
		const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs) {
	return lhs._task == rhs._task && lhs._action == rhs._action;
}
template <class T, class A>
constexpr bool operator!=(
		const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs) {
	return !(lhs == rhs);
}

template <class T, class A>
constexpr bool operator<(
		const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs) {
	if (lhs._task == rhs._task) {
		return lhs._action < rhs._action;
	}
	return lhs._task < rhs._task;
}


template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_predicates(
		fea::span<const PredicateEnum> preds) {
	if (preds.size() + _predicates.size() > _max_preds) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.insert(_predicates.end(), preds.begin(), preds.end());
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_predicates(
		std::initializer_list<PredicateEnum>&& preds) {
	add_predicates({ preds.begin(), preds.end() });
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_predicate(
		PredicateEnum pred) {
	if (_predicates.size() + 1 > _max_preds) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.push_back(pred);
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_subtasks(
		fea::span<const subtask_t> subtasks) {
	_subtasks.insert(_subtasks.end(), subtasks.begin(), subtasks.end());
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_subtasks(
		std::initializer_list<subtask_t>&& subtasks) {
	add_subtasks({ subtasks.begin(), subtasks.end() });
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::add_subtask(
		subtask_t subtask) {
	_subtasks.push_back(subtask);
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
fea::span<const PredicateEnum>
htn_method<TaskEnum, ActionEnum, PredicateEnum>::predicates() const {
	return { _predicates.begin(), _predicates.end() };
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
fea::span<const typename htn_method<TaskEnum, ActionEnum,
		PredicateEnum>::subtask_t>
htn_method<TaskEnum, ActionEnum, PredicateEnum>::subtasks() const {
	return { _subtasks.begin(), _subtasks.end() };
}

template <class TaskEnum, class ActionEnum, class PredicateEnum>
void htn_method<TaskEnum, ActionEnum, PredicateEnum>::validate() const {
	if (_predicates.empty()) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Method requires at least one predicate.");
	}
	if (detail::has_duplicates<_max_preds>(predicates())) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Method predicates should not contain duplicates.");
	}

	if (_subtasks.empty()) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Method requires at least one subtask.");
	}
	// subtasks can have duplicates
}

template <class MethodEnum>
void htn_task<MethodEnum>::add_methods(fea::span<const MethodEnum> methods) {
	if (methods.size() + _methods.size() > _max_methods) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many methods. Maybe you have duplicates?");
	}

	_methods.insert(_methods.end(), methods.begin(), methods.end());
}

template <class MethodEnum>
void htn_task<MethodEnum>::add_methods(
		std::initializer_list<MethodEnum>&& methods) {
	add_methods({ methods.begin(), methods.end() });
}

template <class MethodEnum>
void htn_task<MethodEnum>::add_method(MethodEnum method) {
	if (_methods.size() + 1 > _max_methods) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many methods. Maybe you have duplicates?");
	}
	_methods.push_back(method);
}

template <class MethodEnum>
fea::span<const MethodEnum> htn_task<MethodEnum>::methods() const {
	return { _methods.begin(), _methods.end() };
}

template <class MethodEnum>
void htn_task<MethodEnum>::validate() const {
	if (_methods.empty()) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Task requires at least one method.");
	}
	if (detail::has_duplicates<_max_methods>(methods())) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Task methods should not contain duplicates.");
	}
}


template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr auto
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_task() {
	return task_t{};
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr auto
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_method() {
	return method_t{};
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr auto
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_action() {
	return action_t{};
}


template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <TaskEnum E>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::add_task(task_t&& t) {
	std::get<size_t(E)>(_tasks) = std::move(t);
	validate(E);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <MethodEnum E>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::add_method(method_t&& m) {
	std::get<size_t(E)>(_methods) = std::move(m);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <ActionEnum E>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::add_action(action_t&& a) {
	std::get<size_t(E)>(_actions) = std::move(a);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <PredicateEnum E, class PredFunc>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::add_predicate(PredFunc&& pred_func) {
	std::get<size_t(E)>(_predicates) = std::forward<PredFunc>(pred_func);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <OperatorEnum E, class OpFunc>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::add_operator(OpFunc&& op_func) {
	std::get<size_t(E)>(_operators) = std::forward<OpFunc>(op_func);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_plan(TaskEnum root_task,
		WorldState w) {
	_plan.clear();
	return make_plan_imp(root_task, w);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <class... OpArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::run_plan(WorldState& w,
		OpArgs... op_args) {
	if (_plan.empty()) {
		return true;
	}

	if constexpr (_provide_enum_to_operators) {
		static_assert(sizeof...(OpArgs) == sizeof...(OperatorArgs) - 1,
				"htn : Too many operator arguments provided");
	} else {
		static_assert(fea::all_of_v<std::is_same<OpArgs, OperatorArgs>...>,
				"htn : Mismatch between provided arguments and "
				"expected operator callback arguments");
	}

	if constexpr (std::is_same_v<OperatorRet, void>) {
		return run_plan_async(w, std::forward<OpArgs>(op_args)...);
	} else {
		return run_plan_sync(w, std::forward<OpArgs>(op_args)...);
	}
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::notify_finished(WorldState& w) {
	assert(_current_action != ActionEnum::count);
	const action_t& act = _actions[size_t(_current_action)];

	if constexpr (_provide_enum_to_effects) {
		act.apply_effects(w, _current_action);
	} else {
		act.apply_effects(w);
	}

	// TODO : benchmark and consider reversing vector for pop_back.
	_plan.erase(_plan.begin());
	_current_action = ActionEnum::count;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
fea::span<const ActionEnum>
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::plan() const {
	return { _plan.begin(), _plan.end() };
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
void htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::validate(TaskEnum task_e) const {
	if (task_e == TaskEnum::count) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Invalid task.");
	}

	const task_t& t = _tasks[size_t(task_e)];
	t.validate();

	fea::span<const MethodEnum> methods = t.methods();
	for (MethodEnum m : methods) {
		if (m == MethodEnum::count) {
			fea::maybe_throw<std::invalid_argument>(
					__FUNCTION__, __LINE__, "Invalid method in task.");
		}
		_methods[size_t(m)].validate();

		fea::span<const subtask_t> subtasks = _methods[size_t(m)].subtasks();
		for (subtask_t s : subtasks) {
			if (!s.is_task() && !s.is_action()) {
				fea::maybe_throw<std::invalid_argument>(
						__FUNCTION__, __LINE__, "Invalid subtask in method.");
			}

			if (s.is_action()) {
				_actions[size_t(s.action())].validate();
			} else {
				if (s.task() == task_e) {
					fea::maybe_throw<std::invalid_argument>(__FUNCTION__,
							__LINE__, "Tasks cannot contain themselves.");
				}
				validate(s.task());
			}
		}
	}
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <class... OpArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::run_plan_async(WorldState& w,
		OpArgs... op_args) {
	// Operator should be called only once, and we wait for a
	// notification when we are done with it.

	if (_current_action != ActionEnum::count) {
		// Maybe needs replanning.
		return !satisfied(_current_action, w);
	}

	_current_action = _plan.front();
	if (!satisfied(_current_action, w)) {
		return true;
	}

	const action_t& act = _actions[size_t(_current_action)];
	OperatorEnum current_op = act.operator_e();

	if constexpr (_provide_enum_to_operators) {
		_operators[size_t(current_op)](
				std::forward<OpArgs>(op_args)..., current_op);
	} else {
		_operators[size_t(current_op)](std::forward<OpArgs>(op_args)...);
	}

	return false;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
template <class... OpArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::run_plan_sync(WorldState& w,
		OpArgs... op_args) {
	_current_action = _plan.front();
	if (!satisfied(_current_action, w)) {
		return true;
	}

	const action_t& act = _actions[size_t(_current_action)];
	OperatorEnum current_op = act.operator_e();

	if constexpr (_provide_enum_to_operators) {
		if (!_operators[size_t(current_op)](
					std::forward<OpArgs>(op_args)..., current_op)) {
			// Not done.
			return false;
		}
	} else {
		if (!_operators[size_t(current_op)](std::forward<OpArgs>(op_args)...)) {
			// Not done.
			return false;
		}
	}

	// If op returns true, we are done with current op.
	// Apply effects and move on to next op.
	notify_finished(w);

	if (_plan.empty()) {
		return true;
	}
	return false;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_plan_imp(TaskEnum t,
		WorldState& w) {
	fea::span<const MethodEnum> methods = _tasks[size_t(t)].methods();
	for (MethodEnum m : methods) {
		if (!satisfied(m, w)) {
			continue;
		}

		if (make_plan_imp(m, w)) {
			return true;
		}
	}
	return false;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_plan_imp(MethodEnum m,
		WorldState& w) {
	size_t undo_plan_size = _plan.size();
	WorldState undo_state = w;
	fea::span<const subtask_t> subtasks = _methods[size_t(m)].subtasks();

	for (subtask_t s : subtasks) {
		if (s.is_task()) {
			if (!make_plan_imp(s.task(), w)) {
				_plan.resize(undo_plan_size);
				w = undo_state;
				return false;
			}
		} else {
			if (!make_plan_imp(s.action(), w)) {
				_plan.resize(undo_plan_size);
				w = undo_state;
				return false;
			}
		}
	}
	return true;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::make_plan_imp(ActionEnum a,
		WorldState& w) {
	if (!satisfied(a, w)) {
		return false;
	}

	if constexpr (_provide_enum_to_effects) {
		_actions[size_t(a)].apply_effects_and_expected(w, a);
	} else {
		_actions[size_t(a)].apply_effects_and_expected(w);
	}

	_plan.push_back(a);
	return true;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*,
				EffectArgs...)>::satisfied(fea::span<const PredicateEnum> preds,
		const WorldState& w) const {
	for (PredicateEnum p : preds) {
		if constexpr (_provide_enum_to_predicates) {
			if (!_predicates[size_t(p)](&w, p)) {
				return false;
			}
		} else {
			if (!_predicates[size_t(p)](&w)) {
				return false;
			}
		}
	}
	return true;
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::satisfied(MethodEnum m,
		const WorldState& w) const {
	fea::span<const PredicateEnum> preds = _methods[size_t(m)].predicates();
	return satisfied(preds, w);
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
bool htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::satisfied(ActionEnum a,
		const WorldState& w) const {
	fea::span<const PredicateEnum> preds = _actions[size_t(a)].predicates();
	return satisfied(preds, w);
}


template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr bool
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::needs_predicate_enum() {
	if constexpr (sizeof...(PredicateArgs) == 0) {
		return false;
	} else {
		static_assert(sizeof...(PredicateArgs) == 1,
				"htn : Too many arguments in predicate callback.");
		static_assert(
				std::is_same_v<fea::front_t<PredicateArgs...>, PredicateEnum>,
				"htn : Invalid predicate enum type provided in callback "
				"signature.");
		return true;
	}
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr bool
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::needs_effect_enum() {
	if constexpr (sizeof...(EffectArgs) == 0) {
		return false;
	} else {
		static_assert(sizeof...(EffectArgs) == 1,
				"htn : Too many arguments in effects callback.");
		static_assert(std::is_same_v<fea::front_t<EffectArgs...>, ActionEnum>,
				"htn : Invalid effects enum type provided in callback "
				"signature.");
		return true;
	}
}

template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
constexpr bool
htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)>::needs_operator_enum() {
	if constexpr (sizeof...(OperatorArgs) == 0) {
		return false;
	} else {
		using last_t = fea::back_t<OperatorArgs...>;
		if constexpr (std::is_same_v<last_t, OperatorEnum>) {
			return true;
		} else {
			return false;
		}
	}
}
} // namespace fea

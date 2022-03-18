/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
#include "fea/containers/span.hpp"
#include "fea/containers/stack_vector.hpp"
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <initializer_list>

/*
A Hierarchical Task Network planner.
An htn contains tasks, methods and actions. Some of these use predicates and
operators. At a high level, tasks are a simple "container" of other things. They
represent the instructions to achieve a particular goal. Methods are pure
predicates, which contain actions or other tasks. Actions are the "simplest
and smallest" executable behavior. An htn will in the end, be decomposed into a
list of actions to execute.

Tasks can be triggered by the user or nested in other tasks or methods. Tasks do
not have a predicate, and their immediate children must be methods.

A method contains actions, methods or tasks. The main goal of a method is to be
a predicate for a collection of behaviors. The method will only execute if its
predicate returns true. Multiple methods are considered and prioritized in order
of addition.

An action is a basic operation which applies an effect to your "World". Actions
have optional predicates, which will return true if this action can be taken.
Actions must also contain an operator, which is a function to execute to
"achieve" the effects. The operator is your actual game behavior, and once it
is done executing, the action effects are applied on the state. The htn
hierarchy must always end with primitive tasks, so it can be decomposed into a
plan.

A plan is a simple ordered list (vector) of actions to execute sequentially.

Specifics
This htn implementation will require you provided enums to represent these
various structures. All enums must allways end with the 'count' value, which
represents the number of values for that enum.

The callbacks signatures are as followed :
- Predicates : bool(const YourWorldState*);
- Effects : void(YourWorldState*);
- Operators : bool(UserDefinedT*);

The operator return value is true if operator is done, false if not (applicable
during plan running). By using pointers, the htn can call member functions on
your objects.

*/

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
} // namespace detail

template <class, class, class, class, class, class, class, class>
struct htn;

template <class PredicateEnum, class OperatorEnum, class WorldState>
struct htn_action {
	using effects_func_t = std::function<void(WorldState*)>;
	using expected_effects_func_t = effects_func_t;

	// Add operators to this action.
	// Operators are your game behavior functions, which lead up to the effects
	// later applied.
	void add_operators(fea::span<const OperatorEnum> ops) {
		if (ops.size() + _operators.size() > _max_ops) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many operators provided, do you have duplicates?");
		}
		_operators.insert(_operators.end(), ops.begin(), ops.end());
	}

	// Add operators to this action.
	// Operators are your game behavior functions, which lead up to the effects
	// later applied.
	void add_operators(std::initializer_list<OperatorEnum>&& ops) {
		add_operators({ ops.begin(), ops.end() });
	}

	// Add an operator to this action.
	// Operators are your game behavior functions, which lead up to the effects
	// later applied.
	void add_operator(OperatorEnum op) {
		if (_operators.size() + 1 > _max_ops) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many operators provided, do you have duplicates?");
		}
		_operators.push_back(op);
	}

	// Add the effects function of this action.
	// Effects are applied on the world state, after operator execution.
	template <class EffectFunc>
	void add_effect(EffectFunc&& eff_func) {
		_effects = std::forward<EffectFunc>(eff_func);
	}

	// Add the expected effects function of this action.
	// Expected effects are a way to trick the network into planning further in
	// the future. Expected effects are only applied when planning, not when
	// executing the operators.
	template <class EffectFunc>
	void add_expected_effect(EffectFunc&& eff_func) {
		_expected_effects = std::forward<EffectFunc>(eff_func);
	}

	// Add predicates to this action, optional.
	// This will contribute to method selection.
	void add_predicates(fea::span<const PredicateEnum> preds) {
		if (preds.size() + _predicates.size() > _max_preds) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many predicates provided, do you have duplicates?");
		}
		_predicates.insert(_predicates.end(), preds.begin(), preds.end());
	}

	// Add predicates to this action, optional.
	// This will contribute to method selection.
	void add_predicates(std::initializer_list<PredicateEnum>&& preds) {
		add_predicates({ preds.begin(), preds.end() });
	}

	// Add a predicate to this action, optional.
	// This will contribute to method selection.
	void add_predicate(PredicateEnum pred) {
		if (_predicates.size() + 1 > _max_preds) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many predicates provided, do you have duplicates?");
		}
		_predicates.push_back(pred);
	}

	// Used internally to apply effects and expected effects.
	void apply_effects_and_expected(WorldState& w) const {
		_effects(&w);

		if (_expected_effects) {
			_expected_effects(&w);
		}
	}

	// Used internally to apply effects only (while running plan).
	void apply_effects(WorldState& w) const {
		_effects(&w);
	}

	[[nodiscard]] bool has_effects() const {
		return bool(_effects);
	}

	// This action's predicates.
	[[nodiscard]] fea::span<const PredicateEnum> predicates() const {
		return { _predicates.begin(), _predicates.end() };
	}

	// This action's operators.
	[[nodiscard]] fea::span<const OperatorEnum> operators() const {
		return { _operators.begin(), _operators.end() };
	}

private:
	static constexpr size_t _max_preds = size_t(PredicateEnum::count);
	static constexpr size_t _max_ops = size_t(OperatorEnum::count);

	fea::stack_vector<PredicateEnum, _max_preds> _predicates;
	fea::stack_vector<OperatorEnum, _max_ops> _operators;
	effects_func_t _effects{};
	expected_effects_func_t _expected_effects{};
};

// A subtask can be either a high level task, or a basic action.
template <class TaskEnum, class ActionEnum>
struct htn_subtask {
	constexpr htn_subtask() = default;

	constexpr htn_subtask(TaskEnum t)
			: _task(t) {
		assert(!is_action());
	}
	constexpr htn_subtask(ActionEnum a)
			: _action(a) {
		assert(!is_task());
	}

	[[nodiscard]] constexpr bool is_task() const {
		return _task != TaskEnum::count;
	}
	[[nodiscard]] constexpr bool is_action() const {
		return _action != ActionEnum::count;
	}

	[[nodiscard]] constexpr TaskEnum task() const {
		return _task;
	}
	[[nodiscard]] constexpr ActionEnum action() const {
		return _action;
	}

	template <class T, class A>
	friend constexpr bool operator==(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);
	template <class T, class A>
	friend constexpr bool operator!=(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);

	template <class T, class A>
	friend constexpr bool operator<(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);

private:
	TaskEnum _task = TaskEnum::count;
	ActionEnum _action = ActionEnum::count;
};

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

// A method contains a predicate + tasks and/or actions.
template <class TaskEnum, class ActionEnum, class PredicateEnum>
struct htn_method {
	using subtask_t = htn_subtask<TaskEnum, ActionEnum>;

	htn_method() = default;

	// Add predicates to this method.
	// If these evaluate to true, this method will be chosen.
	void add_predicates(fea::span<const PredicateEnum> preds) {
		if (preds.size() + _predicates.size() > _max_preds) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many predicates provided, do you have duplicates?");
		}
		_predicates.insert(_predicates.end(), preds.begin(), preds.end());
	}

	// Add predicates to this method.
	// If these evaluate to true, this method will be chosen.
	void add_predicates(std::initializer_list<PredicateEnum>&& preds) {
		add_predicates({ preds.begin(), preds.end() });
	}

	// Add a predicates to this method.
	// If the predicates evaluate to true, this method will be chosen.
	void add_predicate(PredicateEnum pred) {
		if (_predicates.size() + 1 > _max_preds) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many predicates provided, do you have duplicates?");
		}
		_predicates.push_back(pred);
	}

	// Add subtasks to this method.
	// These tasks will be executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtasks(fea::span<const subtask_t> subtasks) {
		if (subtasks.size() + _subtasks.size() > _max_subtasks) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many subtasks provided, do you have duplicates?");
		}
		_subtasks.insert(_subtasks.end(), subtasks.begin(), subtasks.end());
	}

	// Add subtasks to this method.
	// These tasks will be executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtasks(std::initializer_list<subtask_t>&& subtasks) {
		add_subtasks({ subtasks.begin(), subtasks.end() });
	}

	// Add a subtask to this method.
	// The tasks are executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtask(subtask_t subtask) {
		if (_subtasks.size() + 1 > _max_subtasks) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many subtasks provided, do you have duplicates?");
		}
		_subtasks.push_back(subtask);
	}

	// Add a predicates to this method.
	// If the predicates evaluate to true, this method will be chosen.
	void add_subtasks(subtask_t subtask) {
		if (_subtasks.size() + 1 > _max_subtasks) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many subtasks provided, do you have duplicates?");
		}
		_subtasks.push_back(subtask);
	}

	[[nodiscard]] fea::span<const PredicateEnum> predicates() const {
		return { _predicates.begin(), _predicates.end() };
	}

	[[nodiscard]] fea::span<const subtask_t> subtasks() const {
		return { _subtasks.begin(), _subtasks.end() };
	}

private:
	static constexpr size_t _max_subtasks
			= size_t(TaskEnum::count) + size_t(ActionEnum::count);
	static constexpr size_t _max_preds = size_t(PredicateEnum::count);

	fea::stack_vector<PredicateEnum, _max_preds> _predicates;
	fea::stack_vector<subtask_t, _max_subtasks> _subtasks;
};


// A task contains methods, nothing else.
template <class MethodEnum>
struct htn_task {

	// Adds methods to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_methods(fea::span<const MethodEnum> methods) {
		if (methods.size() + _methods.size() > _max_methods) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many methods. Maybe you have duplicates?");
		}

		_methods.insert(_methods.end(), methods.begin(), methods.end());
	}

	// Adds methods to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_methods(std::initializer_list<MethodEnum>&& methods) {
		add_methods({ methods.begin(), methods.end() });
	}

	// Add a method to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_method(MethodEnum method) {
		if (_methods.size() + 1 > _max_methods) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many methods. Maybe you have duplicates?");
		}
		_methods.push_back(method);
	}

	[[nodiscard]] fea::span<const MethodEnum> methods() const {
		return { _methods.begin(), _methods.end() };
	}

private:
	static constexpr size_t _max_methods = size_t(MethodEnum::count);

	fea::stack_vector<MethodEnum, _max_methods> _methods;
};


// An htn planner and plan runner.
//
// Provide enums of : tasks, methods, actions, predicates and operators.
// All enums must end with 'count' and use the same underlying type.
//
// The 'WorldState' is the template type we will be inquiring and acting upon
// (T). It should be as small of a structure as possible, since it will be
// copied when planning.
//
// You must provide the arguments for your Operator callback.
// Operators must return a bool, true if finished, false if not. Once
// an operator ends, its effects are applied on the WorldState.
template <class TaskEnum, class MethodEnum, class ActionEnum,
		class PredicateEnum, class OperatorEnum, class WorldState,
		class... OperatorArgs>
struct htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*), bool(OperatorArgs...), void(WorldState*)> {
	using task_t = htn_task<MethodEnum>;
	using method_t = htn_method<TaskEnum, ActionEnum, PredicateEnum>;
	using subtask_t = htn_subtask<TaskEnum, ActionEnum>;
	using action_t = htn_action<PredicateEnum, OperatorEnum, WorldState>;
	using predicate_func_t = std::function<bool(const WorldState*)>;
	using operator_func_t = std::function<bool(OperatorArgs...)>;

	// Helpers so you don't have to type the templates manually.
	static constexpr auto make_task() {
		return task_t{};
	}
	static constexpr auto make_method() {
		return method_t{};
	}
	static constexpr auto make_action() {
		return action_t{};
	}

	// Add a task to the network.
	template <TaskEnum E>
	void add_task(task_t&& t) {
		validate<E>(t);
		std::get<size_t(E)>(_tasks) = std::move(t);
	}

	// Add a method to the network.
	template <MethodEnum E>
	void add_method(method_t&& m) {
		std::get<size_t(E)>(_methods) = std::move(m);
	}

	// Add an action to the network.
	template <ActionEnum E>
	void add_action(action_t&& a) {
		std::get<size_t(E)>(_actions) = std::move(a);
	}

	// Add a predicate function to the network.
	template <PredicateEnum E, class PredFunc>
	void add_predicate(PredFunc&& pred_func) {
		std::get<size_t(E)>(_predicates) = std::forward<PredFunc>(pred_func);
	}

	// Add an operator function to the network.
	template <OperatorEnum E, class OpFunc>
	void add_operator(OpFunc&& op_func) {
		std::get<size_t(E)>(_operators) = std::forward<OpFunc>(op_func);
	}

	// Computes and stores a new plan to execute task.
	// Returns true on success, false on failure to plan.
	// Warning : Make sure your WorldState is small enough, it will be copied
	// throughout.
	[[nodiscard]] bool make_plan(TaskEnum root_task, WorldState w) {
		_plan.clear();
		_plan_runner_op_idx = 0;
		return make_plan(root_task, w, _plan);
	}

	// Run a step in the computed plan (does nothing if no plan).
	// This executes whichever action is next in the plan, once.
	// Once the action completes (its operators all return true), applies the
	// action effects to the world state. If the plan and reality get desynced
	// and the plan needs to be recomputed, returns true without executing.
	[[nodiscard]] bool run_next_action(OperatorArgs... op_args, WorldState& w) {
		if (_plan.empty()) {
			return true;
		}

		ActionEnum a = _plan.front();
		if (!satisfied(a, w)) {
			return true;
		}

		const action_t& act = _actions[size_t(a)];
		fea::span<const OperatorEnum> ops = act.operators();
		assert(_plan_runner_op_idx < ops.size());
		OperatorEnum current_op = ops[size_t(_plan_runner_op_idx)];

		// If op returns true, we are done with current op.
		if (!_operators[size_t(current_op)](
					std::forward<OperatorArgs>(op_args)...)) {
			return false;
		}

		// Apply effects and move on to next op.
		act.apply_effects(w);
		++_plan_runner_op_idx;

		if (_plan_runner_op_idx >= ops.size()) {
			// We have no more ops. Pop action and reset op idx.
			_plan_runner_op_idx = 0;

			// TODO : benchmark and consider reversing vector for pop_back.
			_plan.erase(_plan.begin());

			if (_plan.empty()) {
				return true;
			}
		}

		return false;
	}


	// Returns the computed plan (empty if no plan).
	// Do not store onto the span long-term, as memory will get
	// invalidated.
	[[nodiscard]] fea::span<const ActionEnum> plan() const {
		return { _plan.begin(), _plan.end() };
	}

private:
	// Task overload
	// Returns true on success.
	bool make_plan(
			TaskEnum t, WorldState& w, std::vector<ActionEnum>& plan) const {
		fea::span<const MethodEnum> methods = _tasks[size_t(t)].methods();
		for (MethodEnum m : methods) {
			if (!satisfied(m, w)) {
				continue;
			}

			if (make_plan(m, w, plan)) {
				return true;
			}
		}
		return false;
	}

	// Method overload
	// Returns true on success, returns false if any children is non-executable.
	bool make_plan(
			MethodEnum m, WorldState& w, std::vector<ActionEnum>& plan) const {

		size_t undo_plan_size = plan.size();
		WorldState undo_state = w;
		fea::span<const subtask_t> subtasks = _methods[size_t(m)].subtasks();

		for (subtask_t s : subtasks) {
			if (s.is_task()) {
				if (!make_plan(s.task(), w, plan)) {
					return false;
				}
			} else {
				if (!make_plan(s.action(), w, plan)) {
					plan.resize(undo_plan_size);
					w = undo_state;
					return false;
				}
			}
		}
		return true;
	}

	// Action overload
	// Returns true on success, returns false if any children is non-executable.
	bool make_plan(
			ActionEnum a, WorldState& w, std::vector<ActionEnum>& plan) const {
		if (!satisfied(a, w)) {
			return false;
		}

		_actions[size_t(a)].apply_effects_and_expected(w);
		plan.push_back(a);
		return true;
	}

	bool satisfied(
			fea::span<const PredicateEnum> preds, const WorldState& w) const {
		for (PredicateEnum p : preds) {
			if (!_predicates[size_t(p)](&w)) {
				return false;
			}
		}
		return true;
	}

	bool satisfied(MethodEnum m, const WorldState& w) const {
		fea::span<const PredicateEnum> preds = _methods[size_t(m)].predicates();
		return satisfied(preds, w);
	}

	bool satisfied(ActionEnum a, const WorldState& w) const {
		fea::span<const PredicateEnum> preds = _actions[size_t(a)].predicates();
		return satisfied(preds, w);
	}

	// Validates the added task and everything it uses.
	template <TaskEnum E>
	void validate(const task_t& t) const {
		fea::span<const MethodEnum> methods = t.methods();

		{
			if (methods.empty()) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Task requires at least one method.");
			}
			if (detail::has_duplicates<_method_count>(methods)) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Task methods should not contain duplicates.");
			}
		}

		for (MethodEnum m : methods) {
			if (m == MethodEnum::count) {
				fea::maybe_throw<std::invalid_argument>(
						__FUNCTION__, __LINE__, "Invalid method in task.");
			}

			fea::span<const PredicateEnum> preds
					= _methods[size_t(m)].predicates();

			if (preds.empty()) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Method requires at least one predicate.");
			}
			if (detail::has_duplicates<_pred_count>(preds)) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Method predicates should not contain duplicates.");
			}

			fea::span<const subtask_t> subtasks
					= _methods[size_t(m)].subtasks();

			if (subtasks.empty()) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Method requires at least one subtask.");
			}
			if (detail::has_duplicates<_task_count + _action_count>(subtasks)) {
				fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
						"Method subtasks should not contain duplicates.");
			}

			for (subtask_t s : subtasks) {
				if (!s.is_task() && !s.is_action()) {
					fea::maybe_throw<std::invalid_argument>(__FUNCTION__,
							__LINE__, "Invalid subtask in method.");
				}

				if (s.is_action()) {
					const action_t& act = _actions[size_t(s.action())];

					if (!act.has_effects()) {
						fea::maybe_throw<std::invalid_argument>(__FUNCTION__,
								__LINE__, "Action missing effect.");
					}

					if (detail::has_duplicates<_pred_count>(act.predicates())) {
						fea::maybe_throw<std::invalid_argument>(__FUNCTION__,
								__LINE__,
								"Action predicates should not contain "
								"duplicates.");
					}

					if (detail::has_duplicates<_operator_count>(
								act.operators())) {
						fea::maybe_throw<std::invalid_argument>(__FUNCTION__,
								__LINE__,
								"Action operators should not contain "
								"duplicates.");
					}
				} else {
					// if (_tasks[size_t(s.task())] == task_t{}) {
					//	fea::maybe_throw<std::invalid_argument>(
					//			__FUNCTION__, __LINE__, "Method task invalid.");
					//}
				}
			}
		}
	}

	static_assert(
			std::is_enum_v<TaskEnum>, "htn : TaskEnum must be an enum class");
	static_assert(std::is_enum_v<MethodEnum>,
			"htn : MethodEnum must be an enum class");
	static_assert(std::is_enum_v<ActionEnum>,
			"htn : ActionEnum must be an enum class");
	static_assert(std::is_enum_v<PredicateEnum>,
			"htn : PredicateEnum must be an enum class");
	static_assert(std::is_enum_v<OperatorEnum>,
			"htn : OperatorEnum must be an enum class");

	static constexpr size_t _task_count = size_t(TaskEnum::count);
	static constexpr size_t _method_count = size_t(MethodEnum::count);
	static constexpr size_t _action_count = size_t(ActionEnum::count);
	static constexpr size_t _pred_count = size_t(PredicateEnum::count);
	static constexpr size_t _operator_count = size_t(OperatorEnum::count);

	// We store everything here, to minimize duplication.
	std::array<task_t, _task_count> _tasks{};
	std::array<method_t, _method_count> _methods{};
	std::array<action_t, _action_count> _actions{};
	std::array<predicate_func_t, _pred_count> _predicates{};
	std::array<operator_func_t, _operator_count> _operators{};

	std::vector<ActionEnum> _plan;
	// The index of the currently in-progress operator.
	size_t _plan_runner_op_idx = 0;
};

// template <class TaskEnum, class MethodEnum, class ActionEnum,
//		class OperatorEnum, class WorldState, class... OperatorArgs>
// struct htn_builder<TaskEnum, MethodEnum, ActionEnum, OperatorEnum,
// WorldState, 		bool(OperatorArgs...)> { 	static constexpr auto make_htn()
// { return htn<TaskEnum, MethodEnum, ActionEnum, OperatorEnum, WorldState,
//				bool(OperatorArgs...)>{};
//	}
//};

// template <class T>
// struct itask {
//	virtual bool is_compound() const = 0;
//	virtual std::tuple<bool, size_t> satisfied(
//			const T& t, size_t start_idx = 0) const = 0;
//	virtual const char* name() const = 0;
//};
//
// template <class T>
// struct primitive_task : itask<T> {
//	template <class FuncCond, class FuncOp, class FuncEffect>
//	primitive_task(const char* name, FuncCond&& condition, FuncOp&& operator_,
//			FuncEffect&& effect)
//			: _name(name)
//			, _condition(std::forward<FuncCond>(condition))
//			, _operator(std::forward<FuncOp>(operator_))
//			, _effect(std::forward<FuncEffect>(effect)) {
//	}
//
//	bool is_compound() const override {
//		return false;
//	}
//
//	std::tuple<bool, size_t> satisfied(const T& t, size_t = 0) const override {
//		return { std::invoke(_condition, t), 0 };
//	}
//
//	// returns finished
//	void execute_task(T& t) const {
//		std::invoke(_operator, t);
//	}
//
//	void execute_effect(T& t) const {
//		std::invoke(_effect, t);
//	}
//
//	// get/set
//	const char* name() const override {
//		return _name;
//	}
//
// private:
//	const char* _name{ "" };
//	stdext::inplace_function<bool(const T&)> _condition{};
//	stdext::inplace_function<void(T&)> _operator{};
//	stdext::inplace_function<void(T&)> _effect{};
//};
// FEA_FULFILLS_RULE_OF_5(primitive_task<int>);
//// FEA_FULFILLS_FAST_VECTOR(primitive_task<int>); // inplace_function
//
// template <class T>
// struct method {
//	template <class FuncCond>
//	method(const char* name, FuncCond&& condition,
//			std::vector<itask<T>*>&& sub_tasks)
//			: _name(name)
//			, _condition(std::forward<FuncCond>(condition))
//			, _sub_tasks(std::move(sub_tasks)) {
//	}
//
//	bool satisfied(const T& t) const {
//		return std::invoke(_condition, t);
//	}
//
//	const std::vector<itask<T>*>& sub_tasks() const {
//		return _sub_tasks;
//	}
//
//
// private:
//	const char* _name{ "" };
//	stdext::inplace_function<bool(const T&)> _condition{};
//	std::vector<itask<T>*> _sub_tasks{};
//};
// FEA_FULFILLS_RULE_OF_5(method<int>);
//
// template <class T>
// struct compound_task : itask<T> {
//	compound_task(const char* name, std::vector<method<T>>&& methods)
//			: _name(name)
//			, _methods(std::move(methods)) {
//	}
//
//	bool is_compound() const override {
//		return true;
//	}
//
//	std::tuple<bool, size_t> satisfied(
//			const T& t, size_t start_idx = 0) const override {
//		for (size_t i = start_idx; i < _methods.size(); ++i) {
//			if (_methods[i].satisfied(t))
//				return { true, i };
//		}
//		return { false, 0 };
//	}
//
//	const std::vector<itask<T>*>& sub_tasks(size_t method_idx) const {
//		return _methods[method_idx].sub_tasks();
//	}
//
//	// get/set
//	const char* name() const override {
//		return _name;
//	}
//
// private:
//	const char* _name{ "" };
//	std::vector<method<T>> _methods{};
//};
// FEA_FULFILLS_RULE_OF_5(compound_task<int>);
// FEA_FULFILLS_FAST_VECTOR(compound_task<int>);
//
// template <class T>
// struct plan_runner {
//	plan_runner(std::vector<primitive_task<T>>&& plan)
//			: _tasks(std::move(plan)) {
//
//		fsm_state<transition, state, plan_runner&, T&> running_state;
//		running_state.add_event<fsm_event::on_enter>(
//				[](auto&, plan_runner& p, T& t) {
//					if (p.invalid())
//						return;
//
//					auto tup = p._tasks[p._current_task].satisfied(t);
//					if (!std::get<0>(tup)) {
//						p._invalid = true;
//						return;
//					}
//					p._world_copy = t;
//					p._tasks[p._current_task].execute_task(t);
//				});
//		running_state.add_event<fsm_event::on_update>(
//				[](auto&, plan_runner&, T&) {
//					// T cpy = p._world_copy;
//					// p._tasks[p._current_task].execute_effect(cpy);
//					// if (cpy == t) {
//					//	++p._current_task;
//					//	p._world_copy = t;
//					//	machine.trigger<transition::do_running>(p, t);
//					//}
//				});
//		running_state.add_event<fsm_event::on_exit>(
//				[](auto&, plan_runner& p, T& t) {
//					p._tasks[p._current_task].execute_effect(t);
//					++p._current_task;
//				});
//		running_state.add_transition<transition::do_running, state::running>();
//		_smachine.add_state<state::running>(std::move(running_state));
//	}
//	plan_runner()
//			: plan_runner(std::vector<primitive_task<T>>{}) {
//	}
//
//	// returns success
//	bool update(T& t) {
//		if (invalid())
//			return false;
//
//		_smachine.update(*this, t);
//
//		return !invalid();
//	}
//
//	void finish_current_task() {
//		_smachine.delayed_trigger<transition::do_running>();
//	}
//
//	void print() const {
//		printf("Plan\n");
//		for (const auto r : _tasks) {
//			printf("  '%s'\n", r.name());
//		}
//	}
//
//	// invalidated or done, replan
//	bool invalid() const {
//		return _invalid || _current_task >= _tasks.size() || _tasks.empty();
//	}
//
//	// get/set
//	const std::vector<primitive_task<T>>& tasks() const {
//		return _tasks;
//	}
//
// private:
//	enum class transition : unsigned {
//		do_running,
//		count,
//	};
//	enum class state : unsigned {
//		running,
//		count,
//	};
//	std::vector<primitive_task<T>> _tasks{};
//	fsm<transition, state, plan_runner&, T&> _smachine;
//	T _world_copy;
//	size_t _current_task{ 0 };
//	bool _invalid{ false };
//};
//
// template <class T>
// struct undoer {
//	size_t to_eval_start_idx{ 0 };
//	size_t plan_start_idx{ 0 };
//	size_t plan_end_idx{ 0 };
//	T world_backup;
//	// Used to try other methods if current method fails during evaluation.
//	const compound_task<T>* last_compound{ nullptr };
//	size_t last_satisfied_idx{ 0 };
//};
//
// template <class T>
// inline plan_runner<T> create_plan(T world, const itask<T>* root_task) {
//	assert(root_task != nullptr);
//
//	std::vector<primitive_task<T>> plan{};
//	std::vector<const itask<T>*> tasks_to_evaluate({ root_task });
//	std::vector<undoer<T>> undo_vec{};
//	size_t next_method_idx = std::numeric_limits<size_t>::max();
//
//	// Evaluate hierarchy, decompose compounds and methods into primitive tasks,
//	// push those into final plan.
//	while (tasks_to_evaluate.size() > 0) {
//		const itask<T>* current_t = tasks_to_evaluate.back();
//		tasks_to_evaluate.pop_back();
//
//		bool satisfied = false;
//		size_t idx = 0;
//
//		if (next_method_idx == std::numeric_limits<size_t>::max()) {
//			std::tie(satisfied, idx) = current_t->satisfied(world);
//		} else {
//			// If next_method_idx is set, we are processing more satisfied
// methods
//			// from a previous compound.
//			assert(current_t->is_compound());
//
//			satisfied = true;
//			idx = next_method_idx;
//
//			// No need to recheck method.
//			// std::tie(satisfied, idx)
//			// 		= current_t->satisfied(world, next_method_idx);
//			next_method_idx = std::numeric_limits<size_t>::max();
//		}
//
//		// Method or task wasn't satisfied. Reset plan, eval stack and world to
// last undo state. 		if (!satisfied) { 			const undoer<T>& u =
// undo_vec.back();
//
//			assert(u.to_eval_start_idx <= tasks_to_evaluate.size()
//					&& "something went horribly wrong");
//
//			tasks_to_evaluate.erase(
//					tasks_to_evaluate.begin() + u.to_eval_start_idx,
//					tasks_to_evaluate.end());
//
//			// We store both plan chunk begin and end, since decomposing
// compounds
//			// may require to pop multiple nested compounds (which aren't always
// at the end). 			plan.erase(plan.begin() + u.plan_start_idx,
// plan.begin() + u.plan_end_idx);
//
//			world = u.world_backup;
//
//			// Look into the undo stack. If we are processing a compound,
//			// try to find another method which is satisfied.
//			// If found, we will pop it and process it next.
//			if (u.last_compound) {
//				std::tie(satisfied, idx) = u.last_compound->satisfied(
//						world, u.last_satisfied_idx + 1);
//
//				// Found other method to consider.
//				if (satisfied) {
//					tasks_to_evaluate.push_back(u.last_compound);
//					next_method_idx = idx;
//				}
//			}
//
//			undo_vec.pop_back();
//			continue;
//		}
//
//		if (current_t->is_compound()) {
//			// If we are a compound, decompose our tasks to evaluate.
//			// Push an undo operation in case things aren't satisfied.
//			const compound_task<T>* compt
//					= static_cast<const compound_task<T>*>(current_t);
//
//			// undoer<T> u;
//			undo_vec.push_back({ tasks_to_evaluate.size(), plan.size(),
//					plan.size(), world, compt, idx });
//
//			tasks_to_evaluate.insert(tasks_to_evaluate.end(),
//					compt->sub_tasks(idx).rbegin(),
//					compt->sub_tasks(idx).rend());
//
//		} else {
//			// We have found a candidate.
//			// Apply effects on world, add task to plan and increment the
// compound
//"plan chunk size". 			const primitive_task<T>* primt =
// static_cast<const primitive_task<T>*>(current_t);
// primt->execute_effect(world); 			plan.push_back(*primt);
//			undo_vec.back().plan_end_idx = plan.size();
//		}
//	}
//
//	return plan_runner<T>{ std::move(plan) };
//}

} // namespace fea

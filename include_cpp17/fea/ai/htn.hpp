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
of (TODO : addition?).

An action is a simple operation which applies an effect to your "World". Actions
have optional predicates, which will return true if this action can be taken.
Actions must contain an operator, which is your function to execute. The htn
hierarchy must always end with primitive tasks, so it can be decomposed into a
plan.

A plan is a simple ordered list (vector) of actions to execute sequentially.

Specifics
This htn implementation will require you provided enums to represent these
various structures. All enums must allways end with the 'count' value, which
represents the number of values for that enum.

*/

namespace fea {

template <class, class, class, class, class, class, class>
struct htn;


// A task contains methods, nothing else.
template <class TaskEnum, class MethodEnum, class ActionEnum,
		class OperatorEnum, class WorldState, class... OperatorArgs>
struct htn_task {

	[[nodiscard]] size_t size() const {
		return _methods.size();
	}

private:
	static constexpr size_t _method_count = size_t(MethodEnum::count);

	fea::stack_vector<MethodEnum, _method_count> _methods;
};

template <class TaskEnum, class ActionEnum>
struct htn_task_or_action {
	constexpr htn_task_or_action() = default;

	constexpr htn_task_or_action(TaskEnum t)
			: _task(t) {
		assert(!is_action());
	}
	constexpr htn_task_or_action(ActionEnum a)
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

private:
	TaskEnum _task = TaskEnum::count;
	ActionEnum _action = ActionEnum::count;
};

// A method contains a predicate + tasks and/or actions.
template <class TaskEnum, class ActionEnum, class PredicateEnum,
		class WorldState>
struct htn_method {
	using predicate_func_t = std::function<bool(const WorldState&)>;
	using task_or_action_t = htn_task_or_action<TaskEnum, ActionEnum>;

	htn_method() = default;

	htn_method(std::initializer_list<PredicateEnum>&& preds,
			std::initializer_list<task_or_action_t>&& tasks_or_actions) {

		if (preds.size() > _max_preds) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many predicates. Maybe you have duplicates?");
		}

		if (tasks_or_actions.size() > _max_subtasks) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					"Too many tasks and actions. Maybe you have duplicate sub-"
					"tasks?");
		}

		std::copy(preds.begin(), preds.end(), _predicates.begin());
		std::copy(tasks_or_actions.begin(), tasks_or_actions.end(),
				_sub_tasks.begin());
	}

	//// Add predicates of this method.
	//// If these evaluate to true, this method will be chosen.
	// void add_predicates(fea::span < const PredicateEnum)

	//		// Enables the provided predicates on this utility function.
	//		void add_predicates(fea::span<const PredicateEnum> preds) {
	//	if (preds.size() + _predicate_size > _predicates.size()) {
	//		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
	//				"Too many predicates provided, do you have duplicates?");
	//	}
	//	std::copy(preds.begin(), preds.end(),
	//			_predicates.begin() + _predicate_size);
	//}

	//// Enables the provided predicates on this utility function.
	// void add_predicates(std::initializer_list<PredicateEnum>&& preds) {
	//	add_predicates({ preds.begin(), preds.end() });
	//}

	//// Enables the provided predicate on this utility function.
	// void add_predicate(PredicateEnum pred) {
	//	if (_predicate_size + 1 > _predicates.size()) {
	//		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
	//				"Too many predicates provided, do you have duplicates?");
	//	}
	//	_predicates[_predicate_size] = pred;
	//	++_predicate_size;
	//}

	[[nodiscard]] fea::span<const PredicateEnum> predicates() const {
		return { _predicates.begin(), _predicates.end() };
	}

	//[[nodiscard]] bool satisfied(const WorldState& ws) const {
	//	return _pred(ws);
	//}

	[[nodiscard]] size_t predicate_size() const {
		return _predicates.size();
	}

	[[nodiscard]] size_t subtask_size() const {
		return _sub_tasks.size();
	}

private:
	static constexpr size_t _max_subtasks
			= size_t(TaskEnum::count) + size_t(ActionEnum::count);
	static constexpr size_t _max_preds = size_t(PredicateEnum::count);

	fea::stack_vector<PredicateEnum, _max_preds> _predicates;
	fea::stack_vector<task_or_action_t, _max_subtasks> _sub_tasks;
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
		WorldState, bool(OperatorArgs...)> {
	using operator_func_t = std::function<bool(OperatorArgs...)>;
	using predicate_func_t = std::function<bool(const WorldState&)>;
	using effects_func_t = std::function<void(WorldState&)>;

private:
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

	// maybe not required
	// static_assert(fea::all_of_v<std::is_same<std::underlying_type_t<TaskEnum>,
	//									std::underlying_type_t<MethodEnum>>,
	//					  std::is_same<std::underlying_type_t<TaskEnum>,
	//							  std::underlying_type_t<ActionEnum>>,
	//					  std::is_same<std::underlying_type_t<TaskEnum>,
	//							  std::underlying_type_t<PredicateEnum>>,
	//					  std::is_same<std::underlying_type_t<TaskEnum>,
	//							  std::underlying_type_t<OperatorEnum>>>,
	//		"htn : all enum classes must use the same underlying type");

	static constexpr size_t _task_count = size_t(TaskEnum::count);
	static constexpr size_t _method_count = size_t(MethodEnum::count);
	static constexpr size_t _action_count = size_t(ActionEnum::count);
	static constexpr size_t _operator_count = size_t(OperatorEnum::count);
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

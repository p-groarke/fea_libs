/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include <cassert>
#include <functional>
#include <initializer_list>
#include <vector>

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

Plan Runner
The plan runner portion of the htn can run the plan 2 ways, synchronously or
asynchronously. You choose between these by returning either void or bool from
your operator callback signature.

If your operators return bool, the plan runner executes synchronously. Actions
run until they return true. At that point, the action will be popped and the
next call will execute a new action, until the plan is over.

If on the other hand, your operators return void, the plan runner executes
asynchronously. The runner delegates the responsibility of notifying it
when actions are over. In this mode, you must notify the htn when an operator is
done (by calling notify_finished). An operator callback is also only ever called
once to trigger it. This mode integrates well with state machines.

Specifics
This htn implementation will require you provided enums to represent these
various structures. All enums must allways end with the 'count' value, which
represents the number of values for that enum.

The callbacks signatures are as followed :
- Predicates : bool(const YourWorldState*);
- Effects : void(YourWorldState*);
- Operators : bool(UserDefinedT...);
	or void(UserDefinedT...);

The callback signature can also have an appropriate enum at the end.
If they do, the called from value will be passed on to your callback.
You may mix-and-match which callback signatures you need.
- Predicates : bool(const YourWorldState*, PredicateEnum);
- Effects : void(YourWorldState*, ActionEnum);
- Operators : bool(UserDefinedT..., OperatorEnum)
	or void(UserDefinedT..., OperatorEnum);


The operator return value is true if operator is done, false if not (applicable
during plan running). By using pointers, the htn can call member functions on
your objects.

*/

namespace fea {
template <class, class, class, class, class, class, class, class>
struct htn;

template <class PredicateEnum, class OperatorEnum, class WorldState,
		class... EffectArgs>
struct htn_action {
	using effects_func_t = std::function<void(WorldState*, EffectArgs...)>;
	using expected_effects_func_t = effects_func_t;

	// Add an operator to this action.
	// Operators are your game behavior functions, which lead up to the effects
	// later applied.
	void add_operator(OperatorEnum op);

	// Add the effects function of this action.
	// Effects are applied on the world state, after operator execution.
	template <class EffectFunc>
	void add_effect(EffectFunc&& eff_func);

	// Add the expected effects function of this action.
	// Expected effects are a way to trick the network into planning further in
	// the future. Expected effects are only applied when planning, not when
	// executing the operators.
	template <class EffectFunc>
	void add_expected_effect(EffectFunc&& eff_func);

	// Add predicates to this action, optional.
	// This will contribute to method selection.
	void add_predicates(fea::span<const PredicateEnum> preds);

	// Add predicates to this action, optional.
	// This will contribute to method selection.
	void add_predicates(std::initializer_list<PredicateEnum>&& preds);

	// Add a predicate to this action, optional.
	// This will contribute to method selection.
	void add_predicate(PredicateEnum pred);

	// Used internally to apply effects and expected effects.
	void apply_effects_and_expected(WorldState& w, EffectArgs... args) const;

	// Used internally to apply effects only (while running plan).
	void apply_effects(WorldState& w, EffectArgs... args) const;

	// Does this action contain effects?
	[[nodiscard]]
	bool has_effects() const;

	[[nodiscard]]
	bool has_expected_effects() const;

	// Does this action contain an operator?
	[[nodiscard]]
	bool has_operator() const;

	// This action's predicates.
	[[nodiscard]]
	fea::span<const PredicateEnum> predicates() const;

	// This action's operator.
	// Cannot be named 'operator'.
	[[nodiscard]]
	OperatorEnum operator_e() const;

	// Throws (or dies) when invalid.
	// Checked when adding tasks that use action.
	void validate() const;

private:
	static constexpr size_t _max_preds = size_t(PredicateEnum::count);
	static constexpr size_t _max_ops = size_t(OperatorEnum::count);

	fea::stack_vector<PredicateEnum, _max_preds> _predicates;
	OperatorEnum _operator = OperatorEnum::count;
	effects_func_t _effects{};
	expected_effects_func_t _expected_effects{};
};


// A subtask can be either a high level task, or a basic action.
template <class TaskEnum, class ActionEnum>
struct htn_subtask {
	constexpr htn_subtask() = default;

	// Construct as high level task.
	constexpr htn_subtask(TaskEnum t);

	// Construct as low-level action.
	constexpr htn_subtask(ActionEnum a);

	// Is this a task (contains children)?
	[[nodiscard]]
	constexpr bool is_task() const;

	// Is this a single action?
	[[nodiscard]]
	constexpr bool is_action() const;

	// Get the task enum.
	[[nodiscard]]
	constexpr TaskEnum task() const;

	// Get the action enum.
	[[nodiscard]]
	constexpr ActionEnum action() const;

	// Comparison.
	template <class T, class A>
	friend constexpr bool operator==(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);

	// Comparison.
	template <class T, class A>
	friend constexpr bool operator!=(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);

	// Comparison.
	template <class T, class A>
	friend constexpr bool operator<(
			const htn_subtask<T, A>& lhs, const htn_subtask<T, A>& rhs);

private:
	TaskEnum _task = TaskEnum::count;
	ActionEnum _action = ActionEnum::count;
};


// A method contains a predicate + tasks and/or actions.
template <class TaskEnum, class ActionEnum, class PredicateEnum>
struct htn_method {
	using subtask_t = htn_subtask<TaskEnum, ActionEnum>;

	htn_method() = default;

	// Add predicates to this method.
	// If these evaluate to true, this method will be chosen.
	void add_predicates(fea::span<const PredicateEnum> preds);

	// Add predicates to this method.
	// If these evaluate to true, this method will be chosen.
	void add_predicates(std::initializer_list<PredicateEnum>&& preds);

	// Add a predicates to this method.
	// If the predicates evaluate to true, this method will be chosen.
	void add_predicate(PredicateEnum pred);

	// Add subtasks to this method.
	// These tasks will be executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtasks(fea::span<const subtask_t> subtasks);

	// Add subtasks to this method.
	// These tasks will be executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtasks(std::initializer_list<subtask_t>&& subtasks);

	// Add a subtask to this method.
	// The tasks are executed in order of addition.
	// You may provide any combination of "tasks" or "actions".
	void add_subtask(subtask_t subtask);

	[[nodiscard]]
	fea::span<const PredicateEnum> predicates() const;

	[[nodiscard]]
	fea::span<const subtask_t> subtasks() const;

	// Throws on invalid method.
	// Called when adding tasks.
	void validate() const;

private:
	static constexpr size_t _max_preds = size_t(PredicateEnum::count);

	fea::stack_vector<PredicateEnum, _max_preds> _predicates;
	std::vector<subtask_t> _subtasks;
};


// A task contains methods, nothing else.
template <class MethodEnum>
struct htn_task {

	// Adds methods to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_methods(fea::span<const MethodEnum> methods);

	// Adds methods to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_methods(std::initializer_list<MethodEnum>&& methods);

	// Add a method to this task.
	// Methods that evaluate to true will be chosen, prioritized by order of
	// addition.
	void add_method(MethodEnum method);

	[[nodiscard]]
	fea::span<const MethodEnum> methods() const;

	// Throws on invalid task.
	// Checked when adding tasks.
	void validate() const;

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
		class... PredicateArgs, class OperatorRet, class... OperatorArgs,
		class... EffectArgs>
struct htn<TaskEnum, MethodEnum, ActionEnum, PredicateEnum, OperatorEnum,
		bool(const WorldState*, PredicateArgs...), OperatorRet(OperatorArgs...),
		void(WorldState*, EffectArgs...)> {
	using task_t = htn_task<MethodEnum>;
	using method_t = htn_method<TaskEnum, ActionEnum, PredicateEnum>;
	using subtask_t = htn_subtask<TaskEnum, ActionEnum>;
	using action_t = htn_action<PredicateEnum, OperatorEnum, WorldState,
			EffectArgs...>;
	using predicate_func_t
			= std::function<bool(const WorldState*, PredicateArgs...)>;
	using operator_func_t = std::function<OperatorRet(OperatorArgs...)>;

	// Helper so you don't have to type the templates manually.
	static constexpr auto make_task();

	// Helper so you don't have to type the templates manually.
	static constexpr auto make_method();

	// Helper so you don't have to type the templates manually.
	static constexpr auto make_action();

	// Add a task to the network.
	template <TaskEnum E>
	void add_task(task_t&& t);

	// Add a method to the network.
	template <MethodEnum E>
	void add_method(method_t&& m);

	// Add an action to the network.
	template <ActionEnum E>
	void add_action(action_t&& a);

	// Add a predicate function to the network.
	template <PredicateEnum E, class PredFunc>
	void add_predicate(PredFunc&& pred_func);

	// Add an operator function to the network.
	template <OperatorEnum E, class OpFunc>
	void add_operator(OpFunc&& op_func);

	// Computes and stores a new plan to execute task.
	// Returns true on success, false on failure to plan.
	// Warning : Make sure your WorldState is small enough, it will be copied
	// throughout.
	[[nodiscard]]
	bool make_plan(TaskEnum root_task, WorldState w);

	// Run a step in the computed plan (does nothing if no plan).
	// This executes whichever action is next in the plan, once.
	// If the action completes (its operator has returned true), applies
	// the action effects to the world state. If the plan and reality get
	// desynced and the plan needs to be recomputed, returns true without
	// executing. If the plan is done or empty, also returns true (to
	// recompute).
	template <class... OpArgs>
	[[nodiscard]]
	bool run_plan(WorldState& w, OpArgs... op_args);

	// When running the htn using the notification runner, you must call this
	// function when your operator is done.
	// It will apply its effects and advance the plan.
	void notify_finished(WorldState& w);

	// Returns the computed plan (empty if no plan).
	// Do not hold on to the span long-term, as memory will get
	// invalidated.
	[[nodiscard]]
	fea::span<const ActionEnum> plan() const;

	// Validates the added task and everything it uses.
	void validate(TaskEnum task_e) const;

private:
	template <class... OpArgs>
	[[nodiscard]]
	bool run_plan_async(WorldState& w, OpArgs... op_args);

	template <class... OpArgs>
	[[nodiscard]]
	bool run_plan_sync(WorldState& w, OpArgs... op_args);

	// Task overload
	// Returns true on success.
	bool make_plan_imp(TaskEnum t, WorldState& w);

	// Method overload
	// Returns true on success, returns false if any children is non-executable.
	bool make_plan_imp(MethodEnum m, WorldState& w);

	// Action overload
	// Returns true on success, returns false if any children is non-executable.
	bool make_plan_imp(ActionEnum a, WorldState& w);

	// Are the predicates satisfied.
	bool satisfied(
			fea::span<const PredicateEnum> preds, const WorldState& w) const;

	// Is the method satisfied.
	bool satisfied(MethodEnum m, const WorldState& w) const;

	// Is the action satisfied.
	bool satisfied(ActionEnum a, const WorldState& w) const;

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

	static_assert(
			sizeof...(PredicateArgs) == 1 || sizeof...(PredicateArgs) == 0,
			"htn : Predicate signature must either contain a single enum at "
			"the end, or nothing");
	static_assert(sizeof...(EffectArgs) == 1 || sizeof...(EffectArgs) == 0,
			"htn : Effects signature must either contain a single enum at "
			"the end, or nothing");
	static_assert(std::is_same_v<OperatorRet, void>
						  || std::is_same_v<OperatorRet, bool>,
			"htn : Incorrect operator return value, must be 'void' or 'bool'");

	static constexpr size_t _task_count = size_t(TaskEnum::count);
	static constexpr size_t _method_count = size_t(MethodEnum::count);
	static constexpr size_t _action_count = size_t(ActionEnum::count);
	static constexpr size_t _pred_count = size_t(PredicateEnum::count);
	static constexpr size_t _operator_count = size_t(OperatorEnum::count);

	// Do we need to pass on enums to the callbacks?
	static constexpr bool needs_predicate_enum();

	// Do we need to pass on enums to the callbacks?
	static constexpr bool needs_effect_enum();

	// Do we need to pass on enums to the callbacks?
	static constexpr bool needs_operator_enum();

	static constexpr bool _provide_enum_to_predicates = needs_predicate_enum();
	static constexpr bool _provide_enum_to_effects = needs_effect_enum();
	static constexpr bool _provide_enum_to_operators = needs_operator_enum();

	// We store everything here, to minimize duplication.
	std::array<task_t, _task_count> _tasks{};
	std::array<method_t, _method_count> _methods{};
	std::array<action_t, _action_count> _actions{};
	std::array<predicate_func_t, _pred_count> _predicates{};
	std::array<operator_func_t, _operator_count> _operators{};

	std::vector<ActionEnum> _plan;
	ActionEnum _current_action = ActionEnum::count;
};

} // namespace fea

#include "imp/htn.imp.hpp"

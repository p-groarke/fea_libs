#include <algorithm>
#include <fea/ai/htn.hpp>
#include <fea/state_machines/hfsm.hpp>
#include <gtest/gtest.h>

namespace {
namespace test1 {

enum class task { exist, task1, count };
enum class method { heal, idle, count };
enum class action { heal, idle, count };
enum class predicate { low_hp, can_heal, always_true, always_false, count };
enum class operators { heal_anim, idle_anim, count };

struct ennemy {
	struct htn_state {
		static constexpr float max_hp = 100.f;
		static constexpr float max_mana = 100.f;

		bool has_low_hp() const {
			return hp <= 25.f;
		}

		bool can_heal() const {
			return mana >= 1.f;
		}

		void heal() {
			hp += 50.f;
			mana -= 1.f;
			hp = std::clamp(hp, 0.f, max_hp);
			mana = std::clamp(mana, 0.f, max_mana);
		}

		void idle() {
			++mana;
			mana = std::clamp(mana, 0.f, max_mana);
		}

		float hp = max_hp;
		float mana = 0.f;
	};

	void hurt(float damage) {
		state.hp -= damage;
		state.hp = std::clamp(state.hp, 0.f, state.max_hp);
	}

	bool play_heal_anim() {
		++_heal_anim_frame;

		if (_heal_anim_frame >= 6) {
			_heal_anim_frame = 0;
			return true;
		}
		return false;
	}

	bool play_idle_anim() {
		++_idle_anim_frame;

		if (_idle_anim_frame >= 2) {
			_idle_anim_frame = 0;
			return true;
		}
		return false;
	}

	htn_state state;

private:
	size_t _heal_anim_frame = 0;
	size_t _idle_anim_frame = 0;
};

TEST(htn, example) {
	fea::htn<task, method, action, predicate, operators,
			bool(const ennemy::htn_state*), bool(ennemy*),
			void(ennemy::htn_state*)>
			htn;

	// Setup Predicates
	{
		htn.add_predicate<predicate::low_hp>(&ennemy::htn_state::has_low_hp);
		htn.add_predicate<predicate::can_heal>(&ennemy::htn_state::can_heal);
		htn.add_predicate<predicate::always_true>(
				[](const ennemy::htn_state*) { return true; });
		htn.add_predicate<predicate::always_false>(
				[](const ennemy::htn_state*) { return false; });
	}

	// Setup Operators
	{
		htn.add_operator<operators::heal_anim>(&ennemy::play_heal_anim);
		htn.add_operator<operators::idle_anim>(&ennemy::play_idle_anim);
	}

	// Setup heal method
	{
		auto heal_action = htn.make_action();
		heal_action.add_predicate(predicate::can_heal);
		heal_action.add_operator(operators::heal_anim);
		heal_action.add_effect(&ennemy::htn_state::heal);
		htn.add_action<action::heal>(std::move(heal_action));

		auto heal_method = htn.make_method();
		heal_method.add_predicate(predicate::low_hp);
		heal_method.add_subtasks({ action::heal, action::idle });
		htn.add_method<method::heal>(std::move(heal_method));
	}

	// Setup idle method
	{
		auto idle_action = htn.make_action();
		idle_action.add_operator(operators::idle_anim);
		idle_action.add_effect(&ennemy::htn_state::idle);
		htn.add_action<action::idle>(std::move(idle_action));

		auto idle_method = htn.make_method();
		idle_method.add_predicate(predicate::always_true);
		idle_method.add_subtask(action::idle);
		htn.add_method<method::idle>(std::move(idle_method));
	}

	// Finally, add the task.
	auto exist_task = htn.make_task();
	exist_task.add_methods({ method::heal, method::idle });

	// This is where validation will occur, and if there are errors in the task,
	// this will throw (or die if compiling without exceptions).
	htn.add_task<task::exist>(std::move(exist_task));

	// Now, we create a plan.
	// In this case, we only have one task (exist), but you could have multiple
	// tasks and use other means to choose what to do.
	{
		ennemy my_ennemy;
		EXPECT_TRUE(htn.make_plan(task::exist, my_ennemy.state));

		// The htn can run the plan for you, or you can get the plan and run it
		// yourself.
		fea::span<const action> teh_plan = htn.plan();
		EXPECT_EQ(teh_plan.size(), 1u);
		EXPECT_EQ(teh_plan[0], action::idle);

		// Every call to run_next_action executes the current operator in the
		// plan once. The function returns true if you need to recompute a plan
		// (either reality != expected, or plan is done executing).
		while (!htn.run_plan(my_ennemy.state, &my_ennemy)) {
		}
		EXPECT_EQ(my_ennemy.state.mana, 1.f);

		// Hurt ennemy.
		my_ennemy.hurt(75.f);
		EXPECT_TRUE(htn.make_plan(task::exist, my_ennemy.state));

		// Plan should now include heal.
		teh_plan = htn.plan();
		EXPECT_EQ(teh_plan.size(), 2u);
		EXPECT_EQ(teh_plan[0], action::heal);
		EXPECT_EQ(teh_plan[1], action::idle);

		while (!htn.run_plan(my_ennemy.state, &my_ennemy)) {
		}

		EXPECT_EQ(my_ennemy.state.hp, 75.f);
		EXPECT_EQ(my_ennemy.state.mana, 1.f);
	}
}

TEST(htn, basics) {
	fea::htn<task, method, action, predicate, operators,
			bool(const ennemy::htn_state*), bool(ennemy*),
			void(ennemy::htn_state*)>
			htn;

	{
		auto t = htn.make_task();
#if defined(FEA_NOTHROW) || defined(FEA_DEBUG)
		EXPECT_DEATH(htn.add_task<task::exist>(std::move(t)), "");
#else
		EXPECT_THROW(
				htn.add_task<task::exist>(std::move(t)), std::invalid_argument);
#endif
	}

	// Simple action test.
	{
		fea::htn_action<predicate, operators, ennemy::htn_state> maction;
		EXPECT_TRUE(maction.predicates().empty());
		EXPECT_FALSE(maction.has_operator());
		EXPECT_FALSE(maction.has_effects());
		EXPECT_FALSE(maction.has_expected_effects());

#if defined(FEA_NOTHROW) || defined(FEA_DEBUG)
		EXPECT_DEATH(maction.validate(), "");
#else
		EXPECT_THROW(maction.validate(), std::invalid_argument);
#endif

		maction.add_effect([](ennemy::htn_state* s) { s->hp = 0.f; });
		maction.add_expected_effect(
				[](ennemy::htn_state* s) { s->mana = 100.f; });
		maction.add_operator(operators::idle_anim);
		maction.add_predicate(predicate::always_false);

		EXPECT_EQ(maction.predicates().size(), 1u);
		EXPECT_TRUE(maction.has_operator());
		EXPECT_EQ(maction.operator_e(), operators::idle_anim);
		EXPECT_TRUE(maction.has_effects());
		EXPECT_TRUE(maction.has_expected_effects());
		maction.validate(); // shouldn't throw or die

		ennemy mennemy;
		maction.apply_effects(mennemy.state);
		EXPECT_EQ(mennemy.state.hp, 0.f);
		EXPECT_EQ(mennemy.state.mana, 0.f);

		maction.apply_effects_and_expected(mennemy.state);
		EXPECT_EQ(mennemy.state.hp, 0.f);
		EXPECT_EQ(mennemy.state.mana, 100.f);

		htn.add_action<action::idle>(std::move(maction));
	}

	// Simple helper test.
	{
		fea::htn_subtask<task, action> toa(task::exist);
		EXPECT_TRUE(toa.is_task());
		EXPECT_FALSE(toa.is_action());
		EXPECT_EQ(toa.task(), task::exist);
		EXPECT_EQ(toa.action(), action::count);

		toa = { action::heal };
		EXPECT_FALSE(toa.is_task());
		EXPECT_TRUE(toa.is_action());
		EXPECT_EQ(toa.task(), task::count);
		EXPECT_EQ(toa.action(), action::heal);
	}

	// Simple method test.
	{
		fea::htn_method<task, action, predicate> meth;
		EXPECT_EQ(meth.subtasks().size(), 0u);
		EXPECT_EQ(meth.predicates().size(), 0u);

#if defined(FEA_NOTHROW) || defined(FEA_DEBUG)
		EXPECT_DEATH(meth.validate(), "");
#else
		EXPECT_THROW(meth.validate(), std::invalid_argument);
#endif

		meth.add_predicate(predicate::always_false);
		meth.add_subtasks({ task::task1, action::heal });

		EXPECT_EQ(meth.subtasks().size(), 2u);
		EXPECT_EQ(meth.predicates().size(), 1u);
		meth.validate(); // shouldn't throw or die

		htn.add_method<method::heal>(std::move(meth));
	}

	// Add missing things for final test.
	{
		fea::htn_action<predicate, operators, ennemy::htn_state> maction;
		maction.add_predicate(predicate::can_heal);
		maction.add_effect(&ennemy::htn_state::heal);
		maction.add_operator(operators::heal_anim);
		htn.add_action<action::heal>(std::move(maction));

		fea::htn_method<task, action, predicate> meth;
		meth.add_predicate(predicate::always_true);
		meth.add_subtask(action::idle);
		htn.add_method<method::idle>(std::move(meth));

		fea::htn_task<method> task;
		task.add_method(method::idle);
		htn.add_task<task::task1>(std::move(task));
	}

	// Simple task test.
	{
		fea::htn_task<method> task;
		EXPECT_EQ(task.methods().size(), 0u);
#if defined(FEA_NOTHROW) || defined(FEA_DEBUG)
		EXPECT_DEATH(htn.validate(task::exist, task), "");
#else
		EXPECT_THROW(htn.validate(task::exist, task), std::invalid_argument);
#endif

		task.add_methods({ method::heal, method::idle });
		EXPECT_EQ(task.methods().size(), 2u);
		htn.validate(task::exist, task); // shouldn't throw or die

		htn.add_task<task::exist>(std::move(task));
	}
}
} // namespace test1

namespace test2 {

enum class task { t1, t_true, t_false, count };
enum class meth { m_true, m_true2, m_false, m_false2, count };
enum class act { a_true, a_false, no_pred, count };
enum class pred { always_true, always_false, count };
enum class op { o_true, o_false, no_pred, count };

struct test_state {
	bool op_func(op o) {
		executed_ops.push_back(o);
		return true;
	}

	void effect_func(act a) {
		executed_acts.push_back(a);
	}

	std::vector<op> executed_ops;
	std::vector<act> executed_acts;
};

TEST(htn, planner) {
	fea::htn<task, meth, act, pred, op, bool(const test_state*, pred),
			bool(test_state*, op), void(test_state*, act)>
			htn;

	{
		htn.add_predicate<pred::always_true>(
				[](const test_state*, pred) { return true; });
		htn.add_predicate<pred::always_false>(
				[](const test_state*, pred) { return false; });

		htn.add_operator<op::o_true>(&test_state::op_func);
		htn.add_operator<op::o_false>(&test_state::op_func);
		htn.add_operator<op::no_pred>(&test_state::op_func);
	}

	{
		auto a = htn.make_action();
		a.add_predicate(pred::always_true);
		a.add_operator(op::o_true);
		a.add_effect(&test_state::effect_func);
		htn.add_action<act::a_true>(std::move(a));

		a = htn.make_action();
		a.add_predicate(pred::always_false);
		a.add_operator(op::o_false);
		a.add_effect(&test_state::effect_func);
		htn.add_action<act::a_false>(std::move(a));

		a = htn.make_action();
		a.add_operator(op::no_pred);
		a.add_effect(&test_state::effect_func);
		htn.add_action<act::no_pred>(std::move(a));
	}

	{
		auto m = htn.make_method();
		m.add_predicate(pred::always_true);
		m.add_subtasks({ act::a_true, act::a_true, act::no_pred });
		htn.add_method<meth::m_true>(std::move(m));

		m = htn.make_method();
		m.add_predicate(pred::always_true);
		m.add_subtasks({ act::no_pred, act::no_pred, act::no_pred });
		htn.add_method<meth::m_true2>(std::move(m));

		m = htn.make_method();
		m.add_predicate(pred::always_false);
		m.add_subtasks({ act::a_true, act::a_true, act::a_true, act::no_pred });
		htn.add_method<meth::m_false>(std::move(m));

		m = htn.make_method();
		m.add_predicate(pred::always_true);
		m.add_subtasks({ act::a_true, act::a_true, act::a_true, act::a_false });
		htn.add_method<meth::m_false2>(std::move(m));
	}

	{
		auto t = htn.make_task();
		t.add_methods({ meth::m_false, meth::m_false2, meth::m_true });
		htn.add_task<task::t_true>(std::move(t));

		test_state s;
		EXPECT_TRUE(htn.make_plan(task::t_true, s));
		fea::span<const act> p = htn.plan();

		const std::vector<act> exp{ act::a_true, act::a_true, act::no_pred };
		const std::vector<op> exp_op{ op::o_true, op::o_true, op::no_pred };

		EXPECT_EQ(p, fea::span<const act>{ exp });

		while (!htn.run_plan(s, &s)) {
		}
		EXPECT_EQ(s.executed_acts, exp);
		EXPECT_EQ(s.executed_ops, exp_op);
	}

	{
		auto t = htn.make_task();
		t.add_methods({ meth::m_false2 });
		htn.add_task<task::t_false>(std::move(t));

		test_state s;
		EXPECT_FALSE(htn.make_plan(task::t_false, s));
		EXPECT_TRUE(htn.plan().empty());
	}

	{
		// Use tasks in subtasks.
		auto m = htn.make_method();
		m.add_predicate(pred::always_true);
		m.add_subtasks({ act::a_true, act::no_pred, task::t_false });
		htn.add_method<meth::m_false>(std::move(m));

		auto t = htn.make_task();
		t.add_methods({ meth::m_false, meth::m_true });
		htn.add_task<task::t1>(std::move(t));

		test_state s;
		EXPECT_TRUE(htn.make_plan(task::t1, s));
		fea::span<const act> p = htn.plan();

		const std::vector<act> exp{ act::a_true, act::a_true, act::no_pred };
		const std::vector<op> exp_op{ op::o_true, op::o_true, op::no_pred };

		EXPECT_EQ(p, fea::span<const act>{ exp });

		while (!htn.run_plan(s, &s)) {
		}
		EXPECT_EQ(s.executed_acts, exp);
		EXPECT_EQ(s.executed_ops, exp_op);
	}
}
} // namespace test2

namespace test3 {
struct dude {
private:
	enum class transition : unsigned {
		do_idle,
		do_attack,
		do_chill,
		do_quick_attack,
		count,
	};
	enum class state : unsigned {
		idle,
		attacking,
		chilling,
		quick_attacking,
		count,
	};

	enum class task { attack, root, count };
	enum class method {
		normal_attack,
		double_attack,
		multi_attack,
		chill,
		count
	};
	enum class action { quick_attack, attack, chill, count };
	enum class predicate {
		can_attack,
		can_chill,
		can_double_attack,
		always_true,
		count
	};
	enum class operators { quick_attack, attack, chill, count };

public:
	struct htn_data {
		bool can_attack() const {
			return !on_cooldown && has_enemy && stamina >= 5;
		}
		bool can_double_attack() const {
			return can_attack() && stamina >= 10;
		}
		bool can_chill() const {
			return has_enemy;
		}

		void apply_quick_attack_effects() {
			stamina -= 3;
		}
		void apply_attack_effects() {
			on_cooldown = true;
			stamina -= 5;
		}
		void apply_chill_effects() {
			on_cooldown = false;
			stamina += 2;
		}

		int stamina = 10;
		bool on_cooldown = false;
		bool has_enemy = false;
	};

	dude(const char* name_)
			: name(name_) {

		// State machine setup
		{
			fea::hfsm_state<transition, state, dude&> idle_state{ state::idle,
				"idle" };
			idle_state.add_event<fea::hfsm_event::on_enter>(
					[](auto&, dude&) {});
			idle_state.add_event<fea::hfsm_event::on_update>(
					[](auto&, dude&) {});
			idle_state.add_event<fea::hfsm_event::on_exit>([](auto&, dude&) {});
			idle_state.add_transition<transition::do_quick_attack,
					state::quick_attacking>();
			idle_state.add_transition<transition::do_chill, state::chilling>();
			idle_state.add_guard_transition<transition::do_attack,
					state::attacking>(
					[](dude& d) { return d.ai_data.can_attack(); });
			_smachine.add_state<state::idle>(std::move(idle_state));


			fea::hfsm_state<transition, state, dude&> chill_state{
				state::chilling, "chilling"
			};
			chill_state.add_event<fea::hfsm_event::on_enter>(
					[](auto&, dude& d) {
						printf("'%s' chilling\n", d.name);
						d.do_chill = false;
						d._chill_anim_counter = 0;
					});
			chill_state.add_event<fea::hfsm_event::on_update>(
					[](auto& smachine, dude& d) {
						++d._chill_anim_counter;
						if (d._chill_anim_counter > 2) {
							smachine.template trigger<transition::do_idle>(d);
						}
					});
			chill_state.add_event<fea::hfsm_event::on_exit>(
					[](auto&, dude& d) { d._htn.notify_finished(d.ai_data); });
			chill_state.add_transition<transition::do_idle, state::idle>();
			_smachine.add_state<state::chilling>(std::move(chill_state));


			fea::hfsm_state<transition, state, dude&> attack_state{
				state::attacking, "attacking"
			};
			attack_state.add_event<fea::hfsm_event::on_enter>([](auto&,
																	  dude& d) {
				printf("'%s' attacked '%s'\n", d.name, d.current_ennemy->name);
				d.do_attack = false;
				d.current_ennemy->hp -= 10;
				d.check_ded_ennemy();
			});
			attack_state.add_event<fea::hfsm_event::on_update>(
					[](auto& machine, dude& d) {
						++d._attack_anim_counter;
						if (d._attack_anim_counter > 6) {
							machine.template trigger<transition::do_idle>(d);
						}
					});
			attack_state.add_event<fea::hfsm_event::on_exit>(
					[](auto&, dude& d) { d._htn.notify_finished(d.ai_data); });
			attack_state
					.add_transition<transition::do_chill, state::chilling>();
			attack_state.add_transition<transition::do_idle, state::idle>();
			_smachine.add_state<state::attacking>(std::move(attack_state));


			fea::hfsm_state<transition, state, dude&> quick_attack_state{
				state::quick_attacking, "quick attacking"
			};
			quick_attack_state.add_event<fea::hfsm_event::on_enter>(
					[](auto&, dude& d) {
						printf("'%s' quick attacked '%s'\n", d.name,
								d.current_ennemy->name);
						d.do_quick_attack = false;
						d.current_ennemy->hp -= 5;
						d.check_ded_ennemy();
					});
			quick_attack_state.add_event<fea::hfsm_event::on_update>(
					[](auto& machine, dude& d) {
						machine.template trigger<transition::do_idle>(d);
					});
			quick_attack_state.add_event<fea::hfsm_event::on_exit>(
					[](auto&, dude& d) { d._htn.notify_finished(d.ai_data); });
			quick_attack_state
					.add_transition<transition::do_idle, state::idle>();
			_smachine.add_state<state::quick_attacking>(
					std::move(quick_attack_state));
		}

		// Htn setup.
		{
			_htn.add_predicate<predicate::can_attack>(
					&dude::htn_data::can_attack);
			_htn.add_predicate<predicate::can_chill>(
					&dude::htn_data::can_chill);
			_htn.add_predicate<predicate::can_double_attack>(
					&dude::htn_data::can_double_attack);
			_htn.add_predicate<predicate::always_true>(
					[](const dude::htn_data*) { return true; });

			_htn.add_operator<operators::quick_attack>(&dude::quick_attack);
			_htn.add_operator<operators::attack>(&dude::attack);
			_htn.add_operator<operators::chill>(&dude::chill);

			auto quick_attack_action = _htn.make_action();
			quick_attack_action.add_predicate(predicate::can_attack);
			quick_attack_action.add_operator(operators::quick_attack);
			quick_attack_action.add_effect(
					&dude::htn_data::apply_quick_attack_effects);
			_htn.add_action<action::quick_attack>(
					std::move(quick_attack_action));

			auto attack_action = _htn.make_action();
			attack_action.add_predicate(predicate::can_attack);
			attack_action.add_operator(operators::attack);
			attack_action.add_effect(&dude::htn_data::apply_attack_effects);
			_htn.add_action<action::attack>(std::move(attack_action));

			auto chill_action = _htn.make_action();
			chill_action.add_predicate(predicate::can_chill);
			chill_action.add_operator(operators::chill);
			chill_action.add_effect(&dude::htn_data::apply_chill_effects);
			_htn.add_action<action::chill>(std::move(chill_action));

			auto double_attack_meth = _htn.make_method();
			double_attack_meth.add_predicate(predicate::can_double_attack);
			double_attack_meth.add_subtasks(
					{ action::quick_attack, action::attack, action::chill });
			_htn.add_method<method::double_attack>(
					std::move(double_attack_meth));

			auto normal_attack_meth = _htn.make_method();
			normal_attack_meth.add_predicate(predicate::always_true);
			normal_attack_meth.add_subtasks({ action::attack, action::chill });
			_htn.add_method<method::normal_attack>(
					std::move(normal_attack_meth));

			auto attack_task = _htn.make_task();
			attack_task.add_methods(
					{ method::double_attack, method::normal_attack });
			_htn.add_task<task::attack>(std::move(attack_task));

			auto multi_attack_meth = _htn.make_method();
			multi_attack_meth.add_predicate(predicate::can_attack);
			multi_attack_meth.add_subtasks(
					{ task::attack, task::attack, task::attack });
			_htn.add_method<method::multi_attack>(std::move(multi_attack_meth));

			auto chill_meth = _htn.make_method();
			chill_meth.add_predicate(predicate::always_true);
			chill_meth.add_subtask(action::chill);
			_htn.add_method<method::chill>(std::move(chill_meth));

			auto root_task = _htn.make_task();
			root_task.add_methods({ method::multi_attack, method::double_attack,
					method::normal_attack, method::chill });
			_htn.add_task<task::root>(std::move(root_task));
		}
	}

	void quick_attack() {
		_smachine.template trigger<transition::do_quick_attack>(*this);
	}
	void attack() {
		_smachine.template trigger<transition::do_attack>(*this);
	}
	void chill() {
		_smachine.template trigger<transition::do_chill>(*this);
	}


	bool update() {
		bool ret = _htn.run_plan(ai_data, this);
		_smachine.update(*this);
		return ret;
	}

	void check_ded_ennemy() {
		if (current_ennemy->hp <= 0) {
			printf("'%s' is dead\n", current_ennemy->name);
			current_ennemy = nullptr;
			ai_data.has_enemy = false;
		}
	}
	void set_ennemy(dude* e) {
		current_ennemy = e;
		ai_data.has_enemy = true;
	}

	// Returns success. False if failed.
	bool create_plan() {
		return _htn.make_plan(task::root, ai_data);
	}

	fea::span<const action> plan() const {
		return _htn.plan();
	}

	htn_data ai_data;

private:
	const char* name = "";
	dude* current_ennemy = nullptr;
	int hp = 100;

	bool do_attack = false;
	bool do_chill = false;
	bool do_quick_attack = false;

	fea::hfsm<transition, state, dude&> _smachine;
	// simulate 3d animation length
	size_t _attack_anim_counter = 0;
	size_t _chill_anim_counter = 0;

	fea::htn<task, method, action, predicate, operators,
			bool(const dude::htn_data*), void(dude*), void(dude::htn_data*)>
			_htn;
};


TEST(htn, hfsm_basics) {
	{
		dude hero{ "hero" };
		dude baddie{ "baddie" };
		hero.set_ennemy(&baddie);

		EXPECT_TRUE(hero.create_plan());
		// hero.plan().print();
		// printf("\n");
		EXPECT_FALSE(baddie.create_plan());

		EXPECT_EQ(hero.plan().size(), 3u);
		EXPECT_EQ(baddie.plan().size(), 0u);

		while (!hero.update()) {
		}
	}
}

TEST(htn, multi_plan) {
	{
		dude hero{ "hero" };
		dude baddie{ "baddie" };
		hero.set_ennemy(&baddie);
		// boost stamina
		hero.ai_data.apply_chill_effects();
		hero.ai_data.apply_chill_effects();
		hero.ai_data.apply_chill_effects();
		hero.ai_data.apply_chill_effects();
		hero.ai_data.apply_chill_effects();

		hero.create_plan();
		// printf("\n");
		// hero.plan().print();
		// printf("\n");
		baddie.create_plan();

		EXPECT_EQ(hero.plan().size(), 8u);
		EXPECT_EQ(baddie.plan().size(), 0u);

		while (!hero.update()) {
		}
	}
}
} // namespace test3
} // namespace

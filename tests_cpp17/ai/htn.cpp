#include <algorithm>
#include <fea/ai/htn.hpp>
#include <gtest/gtest.h>

namespace {
enum class task { exist, count };
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
		EXPECT_EQ(teh_plan.size(), 1);
		EXPECT_EQ(teh_plan[0], action::idle);

		while (!htn.run_next_action(&my_ennemy, my_ennemy.state)) {
		}
		EXPECT_EQ(my_ennemy.state.mana, 1.f);

		// Hurt ennemy.
		my_ennemy.hurt(75.f);
		EXPECT_TRUE(htn.make_plan(task::exist, my_ennemy.state));

		// Plan should now include heal.
		teh_plan = htn.plan();
		EXPECT_EQ(teh_plan.size(), 2);
		EXPECT_EQ(teh_plan[0], action::heal);
		EXPECT_EQ(teh_plan[1], action::idle);

		while (!htn.run_next_action(&my_ennemy, my_ennemy.state)) {
		}

		EXPECT_EQ(my_ennemy.state.hp, 75.f);
		EXPECT_EQ(my_ennemy.state.mana, 1.f);
	}
}

TEST(htn, basics) {
	ennemy my_ennemy;

	fea::htn<task, method, action, predicate, operators,
			bool(const ennemy::htn_state*), bool(ennemy*),
			void(ennemy::htn_state*)>
			htn;
	htn;

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

		meth.add_predicate(predicate::always_false);
		meth.add_subtasks({ task::exist, action::heal });

		EXPECT_EQ(meth.subtasks().size(), 2u);
		EXPECT_EQ(meth.predicates().size(), 1u);
	}

	// Simple task test.
	{
		fea::htn_task<method> task;
		EXPECT_EQ(task.methods().size(), 0u);

		task.add_methods({ method::heal, method::idle });
		EXPECT_EQ(task.methods().size(), 2u);
	}
}
} // namespace

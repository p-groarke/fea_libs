#include <fea/ai/htn.hpp>
#include <gtest/gtest.h>

namespace {
struct ennemy {
	struct htn_state {
		float hp = 100.f;
		float mana = 0.f;
	};

	bool heal() {
		if (play_heal_anim()) {
			state.hp += 50.f;
			return true;
		}
		return false;
	}

	bool chill() {
		++state.mana;
		return true;
	}

	htn_state state;

private:
	bool play_heal_anim() {
		++_heal_anim_frame;

		if (_heal_anim_frame == 3) {
			_heal_anim_frame = 0;
			return true;
		}
		return false;
	}

	size_t _heal_anim_frame = 0;
};

TEST(htn, basics) {
	enum class task { exist, count };
	enum class method { heal, chill, count };
	enum class action { heal, count };
	enum class predicate { low_hp, always_true, always_false, count };
	enum class operators { heal, count };

	ennemy my_ennemy;

	fea::htn<task, method, action, operators, predicate, ennemy::htn_state,
			bool(ennemy*)>
			htn;
	htn;

	// Simple method test.
	{
		fea::htn_method<task, action, predicate, ennemy::htn_state> meth(
				{ predicate::always_false }, { task::exist, action::heal });
		EXPECT_EQ(meth.subtask_size(), 2u);
		// EXPECT_FALSE(meth.satisfied(my_ennemy.state));
	}
}
} // namespace

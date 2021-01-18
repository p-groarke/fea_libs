#include <fea/utils/thread.hpp>
#include <gtest/gtest.h>

namespace {
TEST(thread, basics) {
	struct my_obj {
		size_t data{ 0 };
	};

	my_obj obj1;
	my_obj obj2;
	my_obj obj3;

	fea::mtx_safe<my_obj> mt_obj{ obj1 };
	fea::mtx_safe<my_obj*> mt_ptr{ &obj2 };
	fea::mtx_safe<my_obj&> mt_ref{ obj3 };

	std::vector<std::function<void()>> funcs;
	for (size_t i = 0; i < 100; ++i) {
		funcs.push_back([&]() { mt_obj.write([](my_obj& o) { ++o.data; }); });
		funcs.push_back([&]() { mt_ptr.write([](my_obj& o) { ++o.data; }); });
		funcs.push_back([&]() { mt_ref.write([](my_obj& o) { ++o.data; }); });
	}
	fea::parallel_tasks(std::move(funcs));

	mt_obj.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
	mt_ptr.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
	mt_ref.read([&](const my_obj& o) { EXPECT_EQ(o.data, 100u); });
}

} // namespace

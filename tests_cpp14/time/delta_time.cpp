#include <chrono>
#include <cstdio>
#include <fea/time/delta_time.hpp>
#include <gtest/gtest.h>

namespace {
TEST(delta_time, basics) {
	fea::delta_time dt_gen;

	auto dt = dt_gen.compute_dt();
	EXPECT_EQ(dt_gen.dt(), dt);
	EXPECT_EQ(dt_gen.fdt().count(), float(dt.count()));

	auto start_time = std::chrono::steady_clock::now();
	dt_gen.compute_dt();
	dt_gen.compute_dt();
	auto end_time = std::chrono::steady_clock::now();

	auto clock_dt = end_time - start_time;
	EXPECT_LE(dt_gen.ns_dt(), clock_dt);
}
} // namespace

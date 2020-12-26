#include <chrono>
#include <cstdio>
#include <fea/benchmark/benchmark.hpp>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

namespace {

TEST(fea_benchmark, basics) {
	// TODO : This tests nothing, rewrite.

	fea::bench::suite suite;
	suite.title("suite test");
	suite.benchmark("test1 blee", []() { std::this_thread::sleep_for(0.1s); });
	suite.benchmark("test2 blee", []() { std::this_thread::sleep_for(0.2s); });
	suite.benchmark("test2 blee", []() { std::this_thread::sleep_for(0.5s); });
	suite.print();

	suite.title("suite averages");
	suite.average(2);
	suite.benchmark("test1 blee", []() { std::this_thread::sleep_for(0.2s); });

	suite.average(4);
	suite.benchmark("test2 blee", []() { std::this_thread::sleep_for(0.1s); });

	suite.average(10);
	suite.benchmark("test2 blee", []() { std::this_thread::sleep_for(0.05s); });
	suite.print();

	size_t in_between = 0;
	suite.title("suite averages in-between");
	suite.average(1);
	suite.benchmark(
			"test1 blee", []() { std::this_thread::sleep_for(0.2s); },
			[&]() { ++in_between; });

	suite.average(2);
	suite.benchmark(
			"test1 blee", []() { std::this_thread::sleep_for(0.2s); },
			[&]() { ++in_between; });

	suite.average(4);
	suite.benchmark(
			"test2 blee", []() { std::this_thread::sleep_for(0.1s); },
			[&]() { ++in_between; });

	suite.average(10);
	suite.benchmark(
			"test2 blee", []() { std::this_thread::sleep_for(0.05s); },
			[&]() { ++in_between; });

	EXPECT_EQ(in_between, 17u);
	suite.print();
}

} // namespace

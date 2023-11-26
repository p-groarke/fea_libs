#include <fea/utils/platform.hpp>
#if FEA_RELEASE && defined(FEA_BENCHMARKS)
#include "global.hpp"
#include "small_obj.hpp"

#include <array>
#include <chrono>
#include <fea/benchmark/benchmark.hpp>
#include <fea/flat_recurse/flat_recurse.hpp>
#include <gtest/gtest.h>
#include <random>
#include <thread>

namespace fea {
template <>
inline std::pair<small_obj*, small_obj*> children_range(
		small_obj* parent, const void*) {
	if (parent->children.empty()) {
		return { nullptr, nullptr };
	}

	small_obj* beg = &parent->children.front();
	return { beg, beg + parent->children.size() };
}
} // namespace fea

namespace {
size_t node_count(size_t depth, size_t width) {
	size_t ret = 0;
	for (size_t i = 0; i < depth; ++i) {
		ret += size_t(std::pow(width, i));
	}
	return ret;
}

template <class T>
void reserve_split_vec(
		size_t depth, size_t width, std::vector<std::vector<T>>* vec) {
	vec->resize(depth);
	for (size_t i = 0; i < depth; ++i) {
		vec->reserve(size_t(std::pow(width, i)));
	}
}

constexpr bool sleep_between = true;

namespace deep {
#if FEA_RELEASE
size_t depth = 25;
#else
size_t depth = 10;
#endif
size_t width = 2;
size_t num_nodes = node_count(depth, width);
} // namespace deep

namespace wide {
size_t depth = 5;
#if FEA_RELEASE
size_t width = 75;
#else
size_t width = 30;
#endif
size_t num_nodes = node_count(depth, width);
} // namespace wide

TEST(flat_recurse, deep_gather_benchmarks) {
	using namespace deep;
	using namespace std::chrono_literals;
	small_obj root{ nullptr };
	root.create_graph(depth, width);


	std::string title_prefix = "Gather Small Objects - " + std::to_string(depth)
			+ " deep, " + std::to_string(width) + " wide, "
			+ std::to_string(num_nodes) + " nodes";

	{
		std::string title = title_prefix + " (with allocation cost)";

		std::vector<small_obj*> out;
		std::vector<std::vector<small_obj*>> out_split;

		fea::bench::suite suite;
		suite.title(title.c_str());
		suite.average(5);

		if (sleep_between) {
			// Easier profiling
			suite.sleep_between(1s);
		}

		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::gather_depthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (depth)",
				[&]() { fea::gather_depthfirst_flat(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (breadth)",
				[&]() { fea::gather_breadthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (split breadth)",
				[&]() { fea::gather_breadthfirst_staged(&root, &out_split); },
				[&]() {
					EXPECT_EQ(out_split.size(), depth);
					out_split = {};
					out_split.shrink_to_fit();
				});

		suite.print();
	}

	{
		std::string title = title_prefix + " (without allocation cost)";

		std::vector<small_obj*> out;
		out.reserve(num_nodes);
		std::vector<std::vector<small_obj*>> out_split;
		out_split.reserve(depth); // eh

		fea::bench::suite suite;
		suite.title(title.c_str());
		suite.average(5);

		if (sleep_between) {
			// Easier profiling
			suite.sleep_between(500ms);
		}

		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::gather_depthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (depth)",
				[&]() { fea::gather_depthfirst_flat(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (breadth)",
				[&]() { fea::gather_breadthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (split breadth)",
				[&]() { fea::gather_breadthfirst_staged(&root, &out_split); },
				[&]() { EXPECT_EQ(out_split.size(), depth); });

		suite.print();
	}
}

TEST(flat_recurse, wide_gather_benchmarks) {
	using namespace wide;
	using namespace std::chrono_literals;
	small_obj root{ nullptr };
	root.create_graph(depth, width);

	// Easier profiling
	std::this_thread::sleep_for(std::chrono::seconds(1));

	std::string title_prefix = "Gather Small Objects - " + std::to_string(depth)
			+ " deep, " + std::to_string(width) + " wide, "
			+ std::to_string(num_nodes) + " nodes";

	{
		std::string title = title_prefix + " (with allocation cost)";

		std::vector<small_obj*> out;
		std::vector<std::vector<small_obj*>> out_split;

		fea::bench::suite suite;
		suite.title(title.c_str());
		suite.average(5);

		if (sleep_between) {
			suite.sleep_between(500ms);
		}

		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::gather_depthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (depth)",
				[&]() { fea::gather_depthfirst_flat(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (breadth)",
				[&]() { fea::gather_breadthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out = {};
					out.shrink_to_fit();
				});

		suite.benchmark(
				"flat (split breadth)",
				[&]() { fea::gather_breadthfirst_staged(&root, &out_split); },
				[&]() {
					EXPECT_EQ(out_split.size(), depth);
					out_split = {};
					out_split.shrink_to_fit();
				});

		suite.print();
	}

	{
		std::string title = title_prefix + " (without allocation cost)";

		std::vector<small_obj*> out;
		out.reserve(num_nodes);
		std::vector<std::vector<small_obj*>> out_split;
		out_split.reserve(depth);

		fea::bench::suite suite;
		suite.title(title.c_str());
		suite.average(5);

		if (sleep_between) {
			suite.sleep_between(1s);
		}

		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::gather_depthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (depth)",
				[&]() { fea::gather_depthfirst_flat(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (breadth)",
				[&]() { fea::gather_breadthfirst(&root, &out); },
				[&]() {
					EXPECT_EQ(out.size(), num_nodes);
					out.clear();
				});

		suite.benchmark(
				"flat (split breadth)",
				[&]() { fea::gather_breadthfirst_staged(&root, &out_split); },
				[&]() {
					EXPECT_EQ(out_split.size(), depth);
					out_split.clear();
				});

		suite.print();
	}
}
} // namespace

#endif // NDEBUG

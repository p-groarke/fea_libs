#pragma once
#include <fea/functional/flat_recurse.hpp>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>

template <class InputIt, class StatePtr = const void>
inline void test_breadth(InputIt root, StatePtr* data_ptr = nullptr) {
	const InputIt croot = root;

	std::vector<InputIt> ref_vec;
	fea::gather_depthfirst(root, &ref_vec, data_ptr);

	// linear breadth non-const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(root, &breadth_graph, data_ptr);
		EXPECT_EQ(breadth_graph.size(), ref_vec.size());

		size_t next_breadth_start = 1;
		for (size_t i = 0; i < breadth_graph.size(); ++i) {
			auto node = breadth_graph[i];

			using fea::children_range;
			auto range = children_range(node, data_ptr);

			size_t j = 0;
			for (auto it = range.first; it != range.second; ++it) {
				size_t child_pos = j + next_breadth_start;

				// if ((*breadth_graph[child_pos])->id != child->id) {
				//	printf("");
				//}

				EXPECT_EQ(*breadth_graph[child_pos], *it);
				++j;
			}

			next_breadth_start += std::distance(range.first, range.second);
		}
	}

	// linear breadth const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(croot, &breadth_graph, data_ptr);
		EXPECT_EQ(breadth_graph.size(), ref_vec.size());

		size_t next_breadth_start = 1;
		for (size_t i = 0; i < breadth_graph.size(); ++i) {
			const auto node = breadth_graph[i];

			using fea::children_range;
			auto range = children_range(node, data_ptr);

			size_t j = 0;
			for (auto it = range.first; it != range.second; ++it) {
				size_t child_pos = j + next_breadth_start;

				// if ((*breadth_graph[child_pos])->id != child->id) {
				//	printf("");
				//}

				EXPECT_EQ(*breadth_graph[child_pos], *it);
				++j;
			}

			next_breadth_start += std::distance(range.first, range.second);
		}
	}

	// split breadth non-const
	{
		std::vector<std::vector<InputIt>> split_breadth_graph;
		fea::gather_breadthfirst_staged(root, &split_breadth_graph, data_ptr);

		size_t staged_size = 0;
		for (auto& v : split_breadth_graph) {
			staged_size += v.size();
		}
		EXPECT_EQ(staged_size, ref_vec.size());


		for (size_t i = 0; i < split_breadth_graph.size(); ++i) {
			std::vector<InputIt>& vec = split_breadth_graph[i];

			size_t next_breadth_start = 0;

			for (size_t j = 0; j < vec.size(); ++j) {
				const auto node = split_breadth_graph[i][j];

				using fea::children_range;
				auto range = children_range(node, data_ptr);

				size_t k = 0;
				for (auto it = range.first; it != range.second; ++it) {
					size_t child_pos = k + next_breadth_start;

					// if ((*breadth_graph[child_pos])->id != child->id) {
					//	printf("");
					//}

					EXPECT_EQ(*split_breadth_graph[i + 1][child_pos], *it);
					++k;
				}

				next_breadth_start += std::distance(range.first, range.second);
			}
		}
	}

	// split breadth const
	{
		std::vector<std::vector<InputIt>> split_breadth_graph;
		fea::gather_breadthfirst_staged(croot, &split_breadth_graph, data_ptr);

		size_t staged_size = 0;
		for (auto& v : split_breadth_graph) {
			staged_size += v.size();
		}
		EXPECT_EQ(staged_size, ref_vec.size());

		for (size_t i = 0; i < split_breadth_graph.size(); ++i) {
			auto& vec = split_breadth_graph[i];

			size_t next_breadth_start = 0;

			for (size_t j = 0; j < vec.size(); ++j) {
				const auto node = split_breadth_graph[i][j];

				using fea::children_range;
				auto range = children_range(node, data_ptr);

				size_t k = 0;
				for (auto it = range.first; it != range.second; ++it) {
					size_t child_pos = k + next_breadth_start;

					// if (*split_breadth_graph[i + 1][child_pos] != *it) {
					//	printf("");
					//}

					EXPECT_EQ(*split_breadth_graph[i + 1][child_pos], *it);
					++k;
				}

				next_breadth_start += std::distance(range.first, range.second);
			}
		}
	}
}

namespace detail {
template <class InputIt, class StatePtr>
inline void test_depth_flat(
		InputIt root, StatePtr* state_ptr, std::bidirectional_iterator_tag) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> depth_graph;
		fea::gather_depthfirst_flat(root, &depth_graph, state_ptr);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(root, &recursed_depth_graph, state_ptr);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}

	// const
	{
		std::vector<InputIt> depth_graph;
		fea::gather_depthfirst_flat(croot, &depth_graph, state_ptr);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(croot, &recursed_depth_graph, state_ptr);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}
}

template <class InputIt, class StatePtr>
inline void test_depth_flat(
		InputIt root, StatePtr* state_ptr, std::input_iterator_tag) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(root, &breadth_graph, state_ptr);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(root, &recursed_depth_graph, state_ptr);

		EXPECT_EQ(breadth_graph.size(), recursed_depth_graph.size());
		// EXPECT_EQ(depth_graph, recursed_depth_graph);
	}

	// const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(croot, &breadth_graph, state_ptr);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(croot, &recursed_depth_graph, state_ptr);

		EXPECT_EQ(breadth_graph.size(), recursed_depth_graph.size());
		// EXPECT_EQ(breadth_graph, recursed_depth_graph);
	}
}

} // namespace detail

template <class InputIt, class StatePtr = const void>
inline void test_depth(InputIt root, StatePtr* state_ptr = nullptr) {
	detail::test_depth_flat(root, state_ptr,
			typename std::iterator_traits<InputIt>::iterator_category{});
}

namespace detail {

template <class InputIt, class CullPred, class ParentCullPred, class StatePtr>
inline void test_culling_flat_depth(InputIt root, CullPred cull_pred,
		ParentCullPred p_cull_pred, StatePtr* state_ptr,
		std::bidirectional_iterator_tag) {
	std::vector<InputIt> depth_graph;
	fea::gather_depthfirst_flat(root, cull_pred, &depth_graph, state_ptr);

	for (auto it : depth_graph) {
		bool p = cull_pred(it);
		bool pp = p_cull_pred(it);
		EXPECT_FALSE(p);
		EXPECT_FALSE(pp);
	}
}
template <class InputIt, class CullPred, class ParentCullPred, class StatePtr>
inline void test_culling_flat_depth(
		InputIt, CullPred, ParentCullPred, StatePtr*, std::input_iterator_tag) {
}
} // namespace detail

// Culls the graph with provided cullpred.
// Checks that remaining nodes' cullpreds are false.
// Checks that the parent's predicate also evaluated to false (since if it
// didn't, the child shouldn't be in there).
template <class InputIt, class CullPred, class ParentCullPred,
		class StatePtr = const void>
inline void test_culling(InputIt root, CullPred cull_pred,
		ParentCullPred p_cull_pred, StatePtr* state_ptr = nullptr) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(root, cull_pred, &breadth_graph, state_ptr);

		for (auto it : breadth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		std::vector<std::vector<InputIt>> staged_breadth_graph;
		fea::gather_breadthfirst_staged(
				root, cull_pred, &staged_breadth_graph, state_ptr);

		for (auto& v : staged_breadth_graph) {
			for (auto it : v) {
				bool p = cull_pred(it);
				bool pp = p_cull_pred(it);
				EXPECT_FALSE(p);
				EXPECT_FALSE(pp);
			}
		}

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(
				root, &recursed_depth_graph, cull_pred, state_ptr);

		for (auto it : recursed_depth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		detail::test_culling_flat_depth(root, cull_pred, p_cull_pred, state_ptr,
				typename std::iterator_traits<InputIt>::iterator_category{});
	}

	// non-const
	{
		std::vector<InputIt> breadth_graph;
		fea::gather_breadthfirst(croot, cull_pred, &breadth_graph, state_ptr);
		for (auto it : breadth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		std::vector<std::vector<InputIt>> staged_breadth_graph;
		fea::gather_breadthfirst_staged(
				croot, cull_pred, &staged_breadth_graph, state_ptr);

		for (auto& v : staged_breadth_graph) {
			for (auto it : v) {
				bool p = cull_pred(it);
				bool pp = p_cull_pred(it);
				EXPECT_FALSE(p);
				EXPECT_FALSE(pp);
			}
		}

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depthfirst(
				croot, &recursed_depth_graph, cull_pred, state_ptr);
		for (auto it : recursed_depth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		detail::test_culling_flat_depth(croot, cull_pred, p_cull_pred,
				state_ptr,
				typename std::iterator_traits<InputIt>::iterator_category{});
	}
}

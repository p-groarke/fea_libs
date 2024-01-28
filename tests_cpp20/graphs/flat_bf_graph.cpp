#include <algorithm>
#include <compare>
#include <fea/graphs/flat_bf_graph.hpp>
#include <fea/numerics/random.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <numeric>
#include <ranges>
#include <vector>

namespace {
constexpr unsigned sentinel = (std::numeric_limits<unsigned>::max)();
unsigned counter = 0;

template <class T>
bool operator==(std::span<T> lhs, std::span<T> rhs) {
	return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

#if FEA_MACOS
template <class T, class A>
constexpr auto operator<=>(
		const std::vector<T, A>& lhs, const std::vector<T, A>& rhs) noexcept {
	std::strong_ordering ret = std::compare_three_way{}(lhs.size(), rhs.size());
	if (!std::is_eq(ret)) {
		return ret;
	}

	for (size_t i = 0; i < lhs.size(); ++i) {
		ret = std::compare_three_way{}(lhs[i], rhs[i]);
		if (!std::is_eq(ret)) {
			return ret;
		}
	}
	return std::strong_ordering::equal;
}
#endif

struct node {
	[[maybe_unused]] friend auto operator<=>(const node&, const node&)
			= default;

	unsigned id = sentinel;
	unsigned parent = sentinel;
	unsigned breadth = 0;
	unsigned order = 0;
	std::vector<unsigned> children = {};

	unsigned value = 0;
};

template <class K>
std::vector<K> flatten(const std::unordered_map<K, node>& map) {
	// Prime the root nodes using insertion order.
	std::vector<std::vector<K>> breadth_graph(1);
	for (const std::pair<const K, node>& p : map) {
		if (p.second.breadth != 0) {
			continue;
		}
		breadth_graph[0].push_back(p.first);
	}
	// Will work until we start swapping root node positions.
	// TODO : add node std::swap and swap order.
	std::ranges::sort(breadth_graph[0], [&](unsigned lhs, unsigned rhs) {
		return map.at(lhs).order < map.at(rhs).order;
	});

	// Now fill the breadths with children in order.
	for (size_t b = 0; b < breadth_graph.size(); ++b) {
		if (breadth_graph[b].empty()) {
			break;
		}

		breadth_graph.push_back({});
		for (size_t i = 0; i < breadth_graph[b].size(); ++i) {
			unsigned k = breadth_graph[b][i];
			std::vector<unsigned>& new_breadth = breadth_graph[b + 1];
			const std::vector<unsigned>& children = map.at(k).children;
			new_breadth.insert(
					new_breadth.end(), children.begin(), children.end());
		}
	}

	// Flatten out expected_graph.
	std::vector<K> ret;
	ret.reserve(map.size());
	for (size_t i = 0; i < breadth_graph.size(); ++i) {
		for (size_t j = 0; j < breadth_graph[i].size(); ++j) {
			ret.push_back(breadth_graph[i][j]);
		}
	}

	assert(ret.size() == map.size());
	return ret;
}

template <class K>
node insert(K k, std::unordered_map<K, node>& expecter,
		fea::flat_bf_graph<K, node>& graph) {
	// if (!expecter.contains(sentinel)) {
	//	// Always make child of root sentinel to later on get flattened order.

	//}

	node n = { .id = k };
	n.order = counter++;
	expecter.insert({ k, n });
	graph.insert(k, n);
	return n;
};

template <class K>
node insert_child(K parent, K k, std::unordered_map<K, node>& expecter,
		fea::flat_bf_graph<K, node>& graph) {
	if (parent == (std::numeric_limits<K>::max)()) {
		return insert(k, expecter, graph);
	}

	node n = {
		.id = k,
		.parent = parent,
		.breadth = expecter.at(parent).breadth + 1,
		.order = counter++,
	};
	expecter.insert({ k, n });
	expecter.at(parent).children.push_back(k);
	graph.insert(parent, k, n);
	graph.at(parent).children.push_back(k);
	return n;
};

template <class K>
std::vector<node> insert_children(const std::vector<K>& kvec,
		std::unordered_map<K, node>& expecter,
		fea::flat_bf_graph<K, node>& graph) {
	std::vector<node> nodes;
	for (size_t i = 0; i < kvec.size(); ++i) {
		K k = kvec[i];
		nodes.push_back(node{ .id = k });
		nodes.back().order = counter++;
		expecter.insert({ k, nodes.back() });
	}

	graph.insert(kvec, nodes);
	return nodes;
};

template <class K>
std::vector<node> insert_children(K parent, const std::vector<K>& kvec,
		std::unordered_map<K, node>& expecter,
		fea::flat_bf_graph<K, node>& graph) {
	if (parent == sentinel) {
		return insert_children(kvec, expecter, graph);
	}

	std::vector<node> nodes;
	for (size_t i = 0; i < kvec.size(); ++i) {
		K k = kvec[i];
		nodes.push_back(node{
				.id = k,
				.parent = parent,
				.breadth = expecter.at(parent).breadth + 1,
				.order = counter++,
		});

		expecter.insert({ k, nodes.back() });
		expecter.at(parent).children.push_back(k);
		graph.at(parent).children.push_back(k);
	}

	graph.insert(parent, kvec, nodes);
	return nodes;
};

template <class K>
node insert_or_assign(K k, const node& n, std::unordered_map<K, node>& expecter,
		fea::flat_bf_graph<K, node>& graph) {
	node new_n = n;
	new_n.id = k;
	expecter.insert_or_assign(k, new_n);
	graph.insert_or_assign(k, new_n);
	return new_n;
};

TEST(flat_bf_graph, basics) {
	{
		node n1{ 0 };
		node n2{ 0 };
		EXPECT_EQ(n1, n2);
	}

	fea::flat_bf_graph<unsigned, node> g;
	std::unordered_map<unsigned, node> expecter;

	EXPECT_TRUE(g.empty());
	EXPECT_EQ(g.size(), 0);
	EXPECT_EQ(g.max_size(), (std::numeric_limits<unsigned>::max)() - 1u);
	EXPECT_EQ(g.breadth_size(), 0);
	// EXPECT_EQ(g.breadth_size(0), 0);

	g.reserve(100);
	EXPECT_EQ(g.capacity(), 100);
	EXPECT_TRUE(g.empty());
	EXPECT_EQ(g.size(), 0);
	EXPECT_EQ(g.breadth_size(), 0);

	g.shrink_to_fit();
	EXPECT_EQ(g.capacity(), 0);
	EXPECT_TRUE(g.empty());
	EXPECT_EQ(g.size(), 0);
	EXPECT_EQ(g.breadth_size(), 0);
	EXPECT_EQ(g.begin(), g.end());
	EXPECT_EQ(g.cbegin(), g.cend());
	EXPECT_EQ(g.key_begin(), g.key_end());
	EXPECT_EQ(g.key_cbegin(), g.key_cend());

	g.insert(0u, node{});
	g.insert(1u, node{});
	g.insert(2u, node{});
	EXPECT_FALSE(g.empty());
	EXPECT_EQ(g.size(), 3);
	EXPECT_EQ(g.breadth_size(), 1);
	EXPECT_EQ(g.breadth_size(0), 3);
	for (unsigned i = 0; i < 3; ++i) {
		EXPECT_EQ(g.breadth_keys(0)[i], i);
	}
	EXPECT_EQ(g.begin() + 3, g.end());
	EXPECT_EQ(g.cbegin() + 3, g.cend());
	EXPECT_EQ(g.key_begin() + 3, g.key_end());
	EXPECT_EQ(g.key_cbegin() + 3, g.key_cend());

	g.clear();
	EXPECT_TRUE(g.empty());
	EXPECT_EQ(g.size(), 0);
	EXPECT_EQ(g.begin(), g.end());
	EXPECT_EQ(g.cbegin(), g.cend());
	EXPECT_EQ(g.key_begin(), g.key_end());
	EXPECT_EQ(g.key_cbegin(), g.key_cend());
	EXPECT_EQ(g.breadth_size(), 0);
	// EXPECT_EQ(g.breadth_size(0), 0);

	g.insert(std::initializer_list<unsigned>{ 0u, 2u, 4u },
			std::vector<node>{ { 0 }, { 2 }, { 4 } });

	EXPECT_FALSE(g.empty());
	EXPECT_EQ(g.size(), 3);
	EXPECT_EQ(g.breadth_size(), 1);
	EXPECT_EQ(g.breadth_size(0), 3);
	for (unsigned i = 0; i < 3; ++i) {
		EXPECT_EQ(g.breadth_keys(0)[i], i * 2);
	}
	EXPECT_EQ(g.begin() + 3, g.end());
	EXPECT_EQ(g.cbegin() + 3, g.cend());
	EXPECT_EQ(g.key_begin() + 3, g.key_end());
	EXPECT_EQ(g.key_cbegin() + 3, g.key_cend());

	// Add some children.
	for (unsigned k = 0u; k < 6u; k += 2u) {
		unsigned child_k = k + 6u;
		g.insert(k, child_k, node{ child_k });
		child_k = k + 7u;
		g.insert(k, child_k, node{ child_k });
	}
	EXPECT_EQ(g.size(), 9);
	EXPECT_EQ(g.breadth_size(), 2);
	EXPECT_EQ(g.breadth_size(0), 3);
	EXPECT_EQ(g.breadth_size(1), 6);

	//  TODO : make this work, add the loop to erase (currently isn't deleting
	//  children).

	g.erase(0u);
	EXPECT_FALSE(g.empty());
	EXPECT_EQ(g.size(), 6u);
	EXPECT_EQ(g.breadth_size(), 2u);
	EXPECT_EQ(g.breadth_size(0), 2u);
	EXPECT_EQ(g.breadth_size(1), 4u);
	{
		std::vector<unsigned> got(
				g.breadth_keys(0).begin(), g.breadth_keys(0).end());
		std::vector<unsigned> expected{ 2u, 4u };
		EXPECT_EQ(got, expected);

		got = std::vector<unsigned>(
				g.breadth_keys(1).begin(), g.breadth_keys(1).end());
		expected = { 8u, 9u, 10u, 11u };
		EXPECT_EQ(got, expected);
	}
	EXPECT_EQ(g.begin() + 6, g.end());
	EXPECT_EQ(g.cbegin() + 6, g.cend());
	EXPECT_EQ(g.key_begin() + 6, g.key_end());
	EXPECT_EQ(g.key_cbegin() + 6, g.key_cend());

	// g.erase(std::initializer_list<unsigned>{ 2u, 4u });

	// g.clear();
	// EXPECT_TRUE(g.empty());
	// EXPECT_EQ(g.size(), 0);
	// EXPECT_EQ(g.begin(), g.end());
	// EXPECT_EQ(g.cbegin(), g.cend());
	// EXPECT_EQ(g.key_begin(), g.key_end());
	// EXPECT_EQ(g.key_cbegin(), g.key_cend());
	// EXPECT_EQ(g.breadth_size(), 0);
	//  EXPECT_EQ(g.breadth_size(0), 0);
}

TEST(flat_bf_graph, insertion) {
	fea::flat_bf_graph<unsigned, node> g;
	std::unordered_map<unsigned, node> expecter;

	{
		unsigned k = 0u;
		EXPECT_EQ(g.find(k), g.end());
		EXPECT_FALSE(g.contains(k));
		EXPECT_EQ(g.count(k), 0);

		node n = insert(k, expecter, g);
		EXPECT_NE(g.find(k), g.end());
		EXPECT_TRUE(g.contains(k));
		EXPECT_EQ(g.count(k), 1);
		EXPECT_FALSE(g.empty());
		EXPECT_EQ(g.size(), 1);
		EXPECT_EQ(g.at_unchecked(k), n);
		EXPECT_EQ(g.at(k), n);
		EXPECT_EQ(*g.find(k), n);
		EXPECT_EQ(g.children_size(k), 0);

		n = insert_or_assign(k, node{ .value = 42 }, expecter, g);
		EXPECT_NE(g.find(k), g.end());
		EXPECT_TRUE(g.contains(k));
		EXPECT_EQ(g.count(k), 1);
		EXPECT_FALSE(g.empty());
		EXPECT_EQ(g.size(), 1);
		EXPECT_EQ(g.at_unchecked(k), n);
		EXPECT_EQ(g.at(k), n);
		EXPECT_EQ(*g.find(k), n);
		EXPECT_EQ(g.children_size(k), 0);
		EXPECT_EQ(g.breadth_size(), 1);
		EXPECT_EQ(g.breadth_size(0), 1);
		EXPECT_EQ(g.breadth_keys(0)[0], k);
	}

	node n;
	size_t size = g.size();
	for (unsigned i = 1u; i < 10u; ++i) {
		n = insert(i, expecter, g);
		EXPECT_NE(g.find(i), g.end());
		EXPECT_TRUE(g.contains(i));
		EXPECT_EQ(g.count(i), 1);
		EXPECT_FALSE(g.empty());
		EXPECT_EQ(g.size(), ++size);
		EXPECT_EQ(g.breadth_size(), 1);
		EXPECT_EQ(g.breadth_size(0), size);
		EXPECT_EQ(g.breadth_keys(0)[i], i);
		EXPECT_EQ(g.at_unchecked(i), n);
		EXPECT_EQ(g.at(i), n);
		EXPECT_EQ(*g.find(i), n);
	}

	struct info {
		unsigned parent = (std::numeric_limits<unsigned>::max)();
		unsigned key = (std::numeric_limits<unsigned>::max)();
		node value = {};
	};

	{
		std::vector<info> kvs;
		unsigned parent = 5u;
		for (unsigned i = 10u; i < 15u; ++i) {
			n = insert_child(parent, i, expecter, g);
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.size(), ++size);
			EXPECT_EQ(g.breadth_size(), 2);
			EXPECT_EQ(g.breadth_size(1), size - g.breadth_size(0));
			EXPECT_EQ(g.at_unchecked(i), n);
			EXPECT_EQ(g.at(i), n);
			EXPECT_EQ(*g.find(i), n);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, n });
		}

		EXPECT_EQ(g.children_size(parent), 5u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 5u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < children_keys.size(); ++i) {
			EXPECT_EQ(children_keys[i], kvs[i].key);
			EXPECT_EQ(children_vals[i], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i]), kvs[i].parent);
			EXPECT_EQ(g.breadth_keys(1)[i], children_keys[i]);
		}
	}
	{
		std::vector<info> kvs;
		unsigned parent = 4u;
		for (unsigned i = 15u; i < 20u; ++i) {
			n = insert_child(parent, i, expecter, g);
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.size(), ++size);
			EXPECT_EQ(g.breadth_size(), 2);
			EXPECT_EQ(g.breadth_size(1), size - g.breadth_size(0));
			EXPECT_EQ(g.at_unchecked(i), n);
			EXPECT_EQ(g.at(i), n);
			EXPECT_EQ(*g.find(i), n);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, n });
		}

		EXPECT_EQ(g.children_size(parent), 5u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 5u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < children_keys.size(); ++i) {
			EXPECT_EQ(children_keys[i], kvs[i].key);
			EXPECT_EQ(children_vals[i], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i]), kvs[i].parent);
		}
	}
	{
		std::vector<info> kvs;
		unsigned parent = 10u;
		std::vector<unsigned> new_ids(5);
		std::iota(new_ids.begin(), new_ids.end(), 20u); // 20 - 25
		std::vector<node> inserted
				= insert_children(parent, new_ids, expecter, g);

		size += 5u;
		EXPECT_EQ(g.size(), size);
		EXPECT_EQ(g.breadth_size(), 3);
		EXPECT_EQ(g.breadth_size(2),
				size - g.breadth_size(0) - g.breadth_size(1));

		for (const node& mn : inserted) {
			unsigned i = mn.id;
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.at_unchecked(i), mn);
			EXPECT_EQ(g.at(i), mn);
			EXPECT_EQ(*g.find(i), mn);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, mn });
		}

		EXPECT_EQ(g.children_size(parent), 5u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 5u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < children_keys.size(); ++i) {
			EXPECT_EQ(children_keys[i], kvs[i].key);
			EXPECT_EQ(children_vals[i], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i]), kvs[i].parent);
			EXPECT_EQ(g.breadth_keys(2)[i], children_keys[i]);
		}
	}
	{
		std::vector<info> kvs;
		for (unsigned i = 25u; i < 35u; ++i) {
			unsigned parent = i - 25u;
			n = insert_child(parent, i, expecter, g);
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.size(), ++size);
			EXPECT_EQ(g.at_unchecked(i), n);
			EXPECT_EQ(g.at(i), n);
			EXPECT_EQ(*g.find(i), n);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, n });
		}

		for (unsigned i = 0; i < 10u; ++i) {
			unsigned parent = i;
			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_GE(children_keys.size(), 1u);
			EXPECT_EQ(children_keys.size(), children_vals.size());

			auto it = std::find(
					children_keys.begin(), children_keys.end(), kvs[i].key);
			EXPECT_NE(it, children_keys.end());

			auto it2 = std::find(
					children_vals.begin(), children_vals.end(), kvs[i].value);
			EXPECT_NE(it2, children_vals.end());

			EXPECT_EQ(g.parent_key(kvs[i].key), kvs[i].parent);
		}
	}
	{
		std::vector<info> kvs;
		unsigned parent = 20u;
		for (unsigned i = 35u; i < 40u; ++i) {
			n = insert_child(parent, i, expecter, g);
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.size(), ++size);
			EXPECT_EQ(g.at_unchecked(i), n);
			EXPECT_EQ(g.at(i), n);
			EXPECT_EQ(*g.find(i), n);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, n });
		}

		EXPECT_EQ(g.children_size(parent), 5u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 5u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < children_keys.size(); ++i) {
			EXPECT_EQ(children_keys[i], kvs[i].key);
			EXPECT_EQ(children_vals[i], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i]), kvs[i].parent);
		}
	}
	{
		std::vector<info> kvs;
		unsigned parent = 0u;
		for (unsigned i = 40u; i < 45u; ++i) {
			n = insert_child(parent, i, expecter, g);
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.size(), ++size);
			EXPECT_EQ(g.at_unchecked(i), n);
			EXPECT_EQ(g.at(i), n);
			EXPECT_EQ(*g.find(i), n);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, n });
		}

		EXPECT_EQ(g.children_size(parent), 6u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 6u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < kvs.size(); ++i) {
			EXPECT_EQ(children_keys[i + 1], kvs[i].key);
			EXPECT_EQ(children_vals[i + 1], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i + 1]), kvs[i].parent);
		}
	}
	{
		std::vector<info> kvs;
		unsigned parent = 3u;
		std::vector<unsigned> new_ids(5);
		std::iota(new_ids.begin(), new_ids.end(), 45u); // 45 - 50
		std::vector<node> inserted
				= insert_children(parent, new_ids, expecter, g);

		size += 5u;
		EXPECT_EQ(g.size(), size);

		for (const node& mn : inserted) {
			unsigned i = mn.id;
			EXPECT_NE(g.find(i), g.end());
			EXPECT_TRUE(g.contains(i));
			EXPECT_EQ(g.count(i), 1);
			EXPECT_FALSE(g.empty());
			EXPECT_EQ(g.at_unchecked(i), mn);
			EXPECT_EQ(g.at(i), mn);
			EXPECT_EQ(*g.find(i), mn);
			EXPECT_EQ(g.parent_key(i), parent);
			EXPECT_EQ(g.parent_at(i).id, parent);
			EXPECT_NE(std::ranges::find(g.parent_at(i).children, i),
					g.parent_at(i).children.end());

			std::span<const unsigned> children_keys = g.children_keys(parent);
			std::span<const node> children_vals = g.children_at(parent);
			EXPECT_NE(std::ranges::find(children_keys, i), children_keys.end());
			EXPECT_NE(std::ranges::find_if(children_vals,
							  [&](const node& n) { return n.id == i; }),
					children_vals.end());

			kvs.push_back({ parent, i, mn });
		}

		EXPECT_EQ(g.children_size(parent), 6u);
		std::span<const unsigned> children_keys = g.children_keys(parent);
		std::span<const node> children_vals = g.children_at(parent);
		EXPECT_EQ(children_keys.size(), 6u);
		EXPECT_EQ(children_keys.size(), children_vals.size());

		for (size_t i = 0; i < kvs.size(); ++i) {
			EXPECT_EQ(children_keys[i + 1], kvs[i].key);
			EXPECT_EQ(children_vals[i + 1], kvs[i].value);
			EXPECT_EQ(g.parent_key(children_keys[i + 1]), kvs[i].parent);
		}
	}

	// Overall test.
	{
		std::vector<size_t> breadth_count;
		for (const std::pair<const unsigned, node>& p : expecter) {
			unsigned k = p.first;
			const node& mnode = p.second;
			EXPECT_TRUE(g.contains(k));
			EXPECT_EQ(g.children_size(k), mnode.children.size());

			std::span<const unsigned> children_keys = g.children_keys(k);
			EXPECT_TRUE(std::equal(children_keys.begin(), children_keys.end(),
					mnode.children.begin()));

			std::span<const node> children_vals = g.children_at(k);
			EXPECT_TRUE(std::equal(children_vals.begin(), children_vals.end(),
					mnode.children.begin(),
					[&](const node& mn, unsigned expected_id) {
						return mn == g.at(expected_id);
					}));

			if (mnode.breadth >= breadth_count.size()) {
				breadth_count.resize(size_t(mnode.breadth) + 1);
			}
			++breadth_count[mnode.breadth];
		}

		EXPECT_EQ(g.breadth_size(), breadth_count.size());
		for (size_t i = 0; i < breadth_count.size(); ++i) {
			EXPECT_EQ(g.breadth_size(i), breadth_count[i]);
			EXPECT_EQ(g.breadth_keys(i).size(), breadth_count[i]);
			EXPECT_EQ(g.breadth_at(i).size(), breadth_count[i]);
		}

		// Flatten out expected_graph.
		std::vector<unsigned> expected_graph = flatten(expecter);
		EXPECT_EQ(g.size(), expected_graph.size());

		// Check the full graph order vs. expected.
		std::span<const unsigned> keys = g.key_data();
		std::span<const node> values = g.data();
		EXPECT_NE(keys.data(), nullptr);
		EXPECT_NE(values.data(), nullptr);

		for (size_t i = 0; i < g.size(); ++i) {
			EXPECT_EQ(keys[i], expected_graph[i]);
			EXPECT_EQ(values[i], expecter.at(keys[i]));
		}
	}
}

TEST(flat_bf_graph, offsets) {
	fea::flat_bf_graph<unsigned, node> graph;
	std::unordered_map<unsigned, node> expecter;

	insert(0u, expecter, graph);
	std::vector<unsigned> children{ 2u, 3u, 4u };
	insert_children(0u, children, expecter, graph);
	insert(1u, expecter, graph);
	insert_child(1u, 5u, expecter, graph);
	std::span<const unsigned> keys = graph.key_data();
	EXPECT_EQ(keys[5], 5u);
}

TEST(flat_bf_graph, fuzzit) {
	constexpr size_t total_num = 1'000;

	fea::flat_bf_graph<unsigned, node> graph;
	std::unordered_map<unsigned, node> expecter;

	// Start at root.
	unsigned parent = sentinel;

	for (unsigned key = 0; key < total_num; ++key) {
		bool batch = fea::random_val<bool>();
		if (batch) {
			unsigned batch_num = fea::random_val(2u, 5u);
			// batch_num = batch_num > total_num ? total_num : batch_num;

			std::vector<unsigned> new_ids(batch_num);
			std::iota(new_ids.begin(), new_ids.end(), key);
			insert_children(parent, new_ids, expecter, graph);
			key += batch_num - 1;
		} else {
			insert_child(parent, key, expecter, graph);
		}

		// Flatten out expected_graph.
		std::vector<unsigned> expected_graph = flatten(expecter);
		EXPECT_EQ(graph.size(), expected_graph.size());

		// Check the full graph order vs. expected.
		std::span<const unsigned> keys = graph.key_data();
		std::span<const node> values = graph.data();
		EXPECT_NE(keys.data(), nullptr);
		EXPECT_NE(values.data(), nullptr);
		EXPECT_EQ(keys.size(), values.size());

		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned gk = keys[i];
			unsigned ek = expected_graph[i];
			const node& gn = values[i];
			const node& en = expecter.at(ek);
			assert(gk == ek);
			assert(gn == en);
			EXPECT_EQ(gk, ek);
			EXPECT_EQ(gn, en);
		}

		// 25%
		bool add_root = fea::random_val(1, 4) == 1;
		parent = add_root ? sentinel
						  : fea::random_val(0u, unsigned(graph.size() - 1));
	}
}
} // namespace
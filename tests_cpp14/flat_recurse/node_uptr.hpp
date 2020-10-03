#pragma once
#include <limits>
#include <memory>
#include <random>
#include <utility>

struct node_uptr {
	node_uptr(node_uptr* parent);

	void create_graph(const size_t max_depth, const size_t num_children,
			const size_t depth = 0);

	const std::vector<std::unique_ptr<node_uptr>>& children() const;
	std::vector<std::unique_ptr<node_uptr>>& children();

	bool operator==(const node_uptr& other) const;

	void disabled(bool disabled);
	bool disabled() const;

	const node_uptr* parent() const;
	node_uptr* parent();

private:
	size_t _id = std::numeric_limits<size_t>::max();
	node_uptr* _parent = nullptr; // observer_ptr
	std::vector<std::unique_ptr<node_uptr>> _children;
	bool _disabled = false;
};

#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace {
// Utilities
namespace detail {
template <class>
struct is_std_tuple : std::false_type {};
template <class... Ts>
struct is_std_tuple<std::tuple<Ts...>> : std::true_type {};

// Checks whether input T is std::tuple.
template <class T>
inline constexpr bool is_std_tuple_v = is_std_tuple<T>::value;

template <class>
struct is_std_array : std::false_type {};
template <class T, size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

// Checks whether input T is std::array.
template <class T>
inline constexpr bool is_std_array_v = is_std_array<T>::value;

template <class Func, size_t... I>
constexpr void static_for_imp(Func& func, std::index_sequence<I...>) {
	(func(std::integral_constant<size_t, I>{}), ...);
}

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
// Accept it with auto, access the index with '::value'.
template <size_t N, class Func>
constexpr void static_for(Func&& func) {
	detail::static_for_imp(func, std::make_index_sequence<N>{});
}

template <size_t, class T>
using accept_idx = T;

template <class, class>
struct tuple_type_from_count;

template <class T, size_t... Idxes>
struct tuple_type_from_count<T, std::index_sequence<Idxes...>> {
	using type = std::tuple<accept_idx<Idxes, T>...>;
};

// Get the type of a tuple constructed with N * T.
template <class T, size_t N>
using tuple_type_from_count_t = typename tuple_type_from_count<T,
		decltype(std::make_index_sequence<N>{})>::type;

} // namespace detail


// Node map.
namespace detail {
template <class NodeEnum, class... NodeDescriptors>
struct node_map {
	// Our descriptors, for later query.
	using descriptors_t = std::tuple<NodeDescriptors...>;

	// Number of descriptors.
	static constexpr size_t num_nodes = sizeof...(NodeDescriptors);

	// Node id enums.
	static constexpr auto ids = std::array{ NodeDescriptors::id... };

	// Number of inputs for each node, indexed at enum value.
	static constexpr auto num_inputs = std::array{ NodeDescriptors::inputs... };

	//// Number of outputs for each node, indexed at enum value.
	// static constexpr auto num_outputs
	//		= std::array{ NodeDescriptors::outputs... };

	// The nodes' compute functions, indexed at enum value.
	static constexpr auto compute_funcs
			= std::tuple{ NodeDescriptors::compute... };

private:
	template <size_t... Idxes>
	static constexpr bool order_ok(std::index_sequence<Idxes...>) {
		using tup_t = std::tuple<NodeDescriptors...>;
		return ((Idxes == size_t(std::tuple_element_t<Idxes, tup_t>::id))
				&& ...);
	}

	static_assert(sizeof...(NodeDescriptors) == size_t(NodeEnum::count),
			"Descriptor count doesn't match enum 'count', missing "
			"descriptors?");
	static_assert(order_ok(std::make_index_sequence<num_nodes>{}),
			"Descriptor 'id' value must be equal to the index of the "
			"descriptor in this map.");
};
} // namespace detail

enum class node : unsigned {
	add,
	substract,
	average,
	count,
};

struct add_descriptor {
	// Our node identifier.
	static constexpr node id = node::add;
	// Our node name, for vizualization and debugging.
	static constexpr std::string_view name{ "add" };

	// Number of input connections.
	static constexpr size_t inputs = 2;
	// Number of output connections.
	// static constexpr size_t outputs = 1;

	// Our computation function.
	static constexpr auto compute
			= [](const auto& lhs, const auto& rhs) { return lhs + rhs; };
};

struct substract_descriptor {
	static constexpr node id = node::substract;
	static constexpr std::string_view name{ "substract" };
	static constexpr size_t inputs = 2;
	// static constexpr size_t outputs = 1;
	static constexpr auto compute
			= [](const auto& lhs, const auto& rhs) { return lhs - rhs; };
};

struct average_descriptor {
	static constexpr node id = node::average;
	static constexpr std::string_view name{ "average" };
	static constexpr size_t inputs = 0;
	// static constexpr size_t outputs = 1;
	static constexpr auto compute = [](const auto&... v) {
		static_assert(sizeof...(v) != 0, "Division by 0.");
		double denom = double(sizeof...(v));
		auto nom = (v + ...);
		return nom / denom;
	};
};

using node_map = detail::node_map<node, add_descriptor, substract_descriptor,
		average_descriptor>;

template <class... Ins>
struct connection {
	constexpr connection(size_t out, Ins... inputs)
			: in_node_idx(out)
			, out_node_idxes({ size_t(inputs)... }) {
	}

	// template <class T, size_t NumOuts, class... Ts>
	// constexpr void map(
	//		std::tuple<const Ts&...> in, std::array<T, NumOuts>& out) const {
	//	static_for<sizeof...(Ins)>([&, this](auto const_i) { out[outidx] = });

	//	for (size_t outidx : out_node_idxes) {
	//		out[outidx] = in[in_node_idx];
	//	}
	//}

	constexpr size_t max_idx() const {
		return *std::max_element(out_node_idxes.begin(), out_node_idxes.end());
	}

	size_t in_node_idx;
	std::array<size_t, sizeof...(Ins)> out_node_idxes;
};

struct output {
	struct _inputs {
		template <class... Inputs>
		constexpr connection<Inputs...> inputs(Inputs... ins) const {
			return connection{ _out, ins... };
		}

	private:
		friend output;

		constexpr _inputs(size_t out_node_idx)
				: _out(out_node_idx) {
		}

		size_t _out;
	};

	constexpr output(size_t node_idx)
			: _ins(node_idx) {
	}

	constexpr _inputs* operator->() {
		return &_ins;
	}

private:
	_inputs _ins;
};

struct graph1_descriptor {
	// This graph works on doubles.
	using data_type_t = double;

	// Graph of nodes, each array represents a "level" of the graph.
	// Each tuple represents the connections between the previous nodes to the
	// next ones. Use node indexes for connections.
	static constexpr auto graph = std::tuple{
		// The graph will require 4 input values.
		std::array{ node::add, node::add },
		std::tuple{ output(0)->inputs(0, 1), output(1)->inputs(0, 1),
				output(1)->inputs(2, 2) },

		std::array{ node::substract, node::add, node::add },
		std::tuple{ output(0)->inputs(0), output(1)->inputs(0),
				output(2)->inputs(0) },

		std::array{ node::average },
		// And output 1 value.
	};
};


namespace detail {
template <class T, size_t Idx>
constexpr void graph_assert_row() {
	if constexpr (Idx % 2 != 0) {
		static_assert(is_std_tuple_v<T>,
				"Odd graph rows must be tuples of connections.");
	} else {
		static_assert(is_std_array_v<T>,
				"Even graph rows must be arrays of node enums.");
	}
}

template <class... Ts, size_t... Idxes>
constexpr void graph_assert_rows(
		const std::tuple<Ts...>&, std::index_sequence<Idxes...>) {
	(graph_assert_row<Ts, Idxes>(), ...);
}

template <class GraphDescriptor, size_t Row = 0>
constexpr size_t num_inputs() {
	constexpr auto row = std::get<Row>(GraphDescriptor::graph);
	static_assert(is_std_array_v<std::decay_t<decltype(row)>>,
			"First row must be array.");

	size_t ret = 0;
	static_for<row.size()>(
			[&](auto const_i) { ret += ::node_map::num_inputs[const_i]; });
	return ret;
}

template <class GraphDescriptor,
		size_t Row = std::tuple_size_v<decltype(GraphDescriptor::graph)> - 1>
constexpr size_t num_outputs() {
	constexpr auto row = std::get<Row>(GraphDescriptor::graph);
	static_assert(is_std_array_v<std::decay_t<decltype(row)>>,
			"Last row must be array.");

	return row.size();
}

template <class GraphDescriptor, size_t RowIdx, size_t... Idxes>
constexpr auto node_array_to_integral_constant_tuple(
		std::index_sequence<Idxes...>) {
	static_assert(RowIdx % 2 == 0);
	constexpr auto arr = std::get<RowIdx>(GraphDescriptor::graph);
	using arr_t = std::decay_t<decltype(arr)>;
	return std::tuple{
		std::integral_constant<size_t, size_t(std::get<Idxes>(arr))>{}...
	};
};

template <class GraphDescriptor, size_t RowIdx, size_t... Idxes>
constexpr auto num_connection_outputs(std::index_sequence<Idxes...>) {
	static_assert(RowIdx % 2 != 0);
	constexpr auto tup = std::get<RowIdx>(GraphDescriptor::graph);
	using tup_t = std::decay_t<decltype(tup)>;
	constexpr auto max_idxes = std::array{ std::get<Idxes>(tup).max_idx()... };
	return *std::max_element(max_idxes.begin(), max_idxes.end()) + 1;
};

template <class GraphDescriptor, size_t RowIdx>
constexpr auto compute_row() {
	static_assert(RowIdx % 2 == 0, "compute_row only works on node rows");

	// Make a tuple type of the inputs.
	using data_type_t = typename GraphDescriptor::data_type_t;
	constexpr size_t num_ins = num_inputs<GraphDescriptor, RowIdx>();
	using row_in_t = tuple_type_from_count_t<const data_type_t&, num_ins>;

	constexpr auto compute_func = [](row_in_t input) {
		// Get the information we need.
		constexpr auto node_array = std::get<RowIdx>(GraphDescriptor::graph);
		constexpr auto node_ic_tuple
				= node_array_to_integral_constant_tuple<GraphDescriptor,
						RowIdx>(std::make_index_sequence<node_array.size()>{});

		// Make a tuple of compute functions.
		constexpr auto compute_funcs = std::apply(
				[](auto... n) {
					return std::tuple{ std::get<n()>(
							::node_map::compute_funcs)... };
				},
				node_ic_tuple);

		constexpr auto num_inputs = std::apply(
				[](auto... n) {
					return std::array{ ::node_map::num_inputs[size_t(n)]... };
				},
				node_array);

		// return std::apply(
		//		[&](const auto&... f) {
		//			return std::apply(
		//					[&](const auto&... v) {
		//						return std::tuple{ f(v)... };
		//					},
		//					input);
		//		},
		//		compute_funcs);
	};

	// Get the connections tuple.
	constexpr auto connections = std::get<RowIdx + 1>(GraphDescriptor::graph);
	using connections_t = std::decay_t<decltype(connections)>;
	constexpr size_t num_connections = std::tuple_size_v<connections_t>;

	// Make a tuple type of the this step's outputs.
	constexpr size_t num_route_in = num_outputs<GraphDescriptor, RowIdx>();
	using route_in_t
			= tuple_type_from_count_t<const data_type_t&, num_route_in>;
	// static_assert(num_connections == std::tuple_size_v<route_in_t>,
	//		"Invalid number of connections.");

	// Make a tuple type of the connections output (next steps input).
	constexpr size_t num_route_out
			= num_connection_outputs<GraphDescriptor, RowIdx + 1>(
					std::make_index_sequence<num_connections>{});
	using route_out_t = std::array<data_type_t, num_route_out>;

	// Make a function that accepts the output array and routes the
	// values to an input array.
	// constexpr auto route_func = [](route_in_t in, route_out_t& out) {
	//	const auto& connects = std::get<RowIdx + 1>(GraphDescriptor::graph);
	//	std::apply([&](const auto&... c) { (c.map(in, out), ...); }, connects);
	//};
	// return route_func;
}

} // namespace detail

template <class InputTup, class OutputTup, class ComputeFunction>
struct graph {
	using input_tuple_t = InputTup;
	using output_tuple_t = OutputTup;

	constexpr graph(ComputeFunction func)
			: _compute_func(func) {
	}

	constexpr OutputTup compute(InputTup input) const {
		return _compute_func(input);
	}

private:
	ComputeFunction _compute_func;
};

template <class GraphDescriptor>
constexpr auto make_graph() {
	// First, assert everything is gucci.
	constexpr size_t num_rows
			= std::tuple_size_v<decltype(GraphDescriptor::graph)>;
	detail::graph_assert_rows(
			GraphDescriptor::graph, std::make_index_sequence<num_rows>{});

	// Create our graph input tuple type using first row.
	using data_type_t = typename GraphDescriptor::data_type_t;
	constexpr size_t num_inputs = detail::num_inputs<GraphDescriptor>();
	using input_tup_t
			= detail::tuple_type_from_count_t<const data_type_t&, num_inputs>;

	// Create our graph output tuple type using last row.
	constexpr size_t num_outputs = detail::num_outputs<GraphDescriptor>();
	using output_tup_t
			= detail::tuple_type_from_count_t<data_type_t, num_outputs>;

	// Create the function that will compute everything.
	constexpr auto func = [](input_tup_t) { return output_tup_t{}; };

	// std::apply(
	//		[](const auto&...) {

	//		},
	//		GraphDescriptor::graph);

	// constexpr auto test = detail::compute_row<GraphDescriptor, 0>();

	// Return an executable graph.
	return graph<input_tup_t, output_tup_t, decltype(func)>{ func };
}

TEST(blog, nodes) {
	constexpr auto g1 = make_graph<graph1_descriptor>();

	// Demo asserts.
	using graph_t = std::decay_t<decltype(g1)>;

	using expected_in_t = std::tuple<const double&, const double&,
			const double&, const double&>;
	static_assert(
			std::is_same_v<expected_in_t, typename graph_t::input_tuple_t>);

	using expected_out_t = std::tuple<double>;
	static_assert(
			std::is_same_v<expected_out_t, typename graph_t::output_tuple_t>);
}
} // namespace

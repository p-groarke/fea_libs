#include <gtest/gtest.h>

#include <algorithm>
#include <iterator>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <vector>

#include <fea/meta/traits.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/utility/error.hpp>
#include <fea/utility/platform.hpp>

namespace fea {
namespace detail {
template <class First, class... Args>
concept vertical_ranges = requires(First, Args...) {
	requires std::ranges::range<First>;
	requires(std::ranges::range<Args> && ...);
	requires(std::is_same_v<
					 fea::iterator_difference_t<std::ranges::iterator_t<First>>,
					 fea::iterator_difference_t<std::ranges::iterator_t<Args>>>
			 && ...);
	requires(std::is_same_v<
					 fea::iterator_category_t<std::ranges::iterator_t<First>>,
					 fea::iterator_category_t<std::ranges::iterator_t<Args>>>
			 && ...);
	// requires(std::is_same_v<
	//				 fea::iterator_concept_t<std::ranges::iterator_t<First>>,
	//				 fea::iterator_concept_t<std::ranges::iterator_t<Args>>>
	//		 && ...);
};

template <class First, class... Args>
concept vertical_iters = requires(First, Args...) {
	requires std::input_or_output_iterator<First>;
	requires(std::input_or_output_iterator<Args> && ...);
	requires(std::is_same_v<fea::iterator_difference_t<First>,
					 fea::iterator_difference_t<Args>>
			 && ...);
	requires(std::is_same_v<fea::iterator_category_t<First>,
					 fea::iterator_category_t<Args>>
			 && ...);
	// requires(std::is_same_v<fea::iterator_concept_t<First>,
	//				 fea::iterator_concept_t<Args>>
	//		 && ...);
};

// Base class defines the iterator tuple and member types.
template <class First, class... Args>
	requires detail::vertical_iters<First, Args...>
struct vert_iterator_base {
	using difference_type = std::ptrdiff_t;

	// Tuple of references (a-la std::tie).
	using value_type = std::tuple<fea::iterator_reference_t<First>,
			fea::iterator_reference_t<Args>...>;

	// Tuple of pointers.
	using pointer = std::tuple<fea::iterator_pointer_t<First>,
			fea::iterator_pointer_t<Args>...>;

	// References don't exist.
	using reference = value_type;

	// Implemented by children.
	using iterator_category = void;
	using iterator_concept = void;

	vert_iterator_base() = default;
	vert_iterator_base(First first_it, Args... args)
			: _iter_tup(first_it, args...) {
	}

	// A tuple of iterators, synchronized.
	std::tuple<First, Args...> _iter_tup;
};

template <class First, class... Args>
struct vert_input_iterator : vert_iterator_base<First, Args...> {

	using base_t = vert_iterator_base<First, Args...>;
	using base_t::base_t;

	using difference_type = typename base_t::difference_type;
	using value_type = typename base_t::value_type;
	using pointer = typename base_t::pointer;
	using reference = typename base_t::reference;

	using iterator_category = std::input_iterator_tag;
	using iterator_concept = std::input_iterator_tag;

	value_type operator*() const {
		return std::apply(
				[](const auto&... args) { return value_type{ *args... }; },
				this->_iter_tup);
	}

	vert_input_iterator& operator++() {
		std::apply([](auto&... args) { (++args, ...); }, this->_iter_tup);
		return *this;
	}

	void operator++(int) {
		++*this;
	}
};
static_assert(std::input_iterator<
		vert_input_iterator<typename std::vector<int>::const_iterator>>);

template <class First, class... Args>
struct vert_forward_iterator : vert_input_iterator<First, Args...> {

	using base_t = vert_input_iterator<First, Args...>;
	using base_t::base_t;

	using difference_type = typename base_t::difference_type;
	using value_type = typename base_t::value_type;
	using pointer = typename base_t::pointer;
	using reference = typename base_t::reference;

	using iterator_category = std::forward_iterator_tag;
	using iterator_concept = std::forward_iterator_tag;

	using base_t::operator*;

	vert_forward_iterator& operator++() {
		this->base_t::operator++();
		return *this;
	}

	vert_forward_iterator operator++(int) {
		vert_forward_iterator ret = *this;
		++*this;
		return ret;
	}

	bool operator==(const vert_forward_iterator& cmp) const {
		return std::apply(
				[&](const auto&... my_its) -> bool {
					return std::apply(
							[&](const auto&... cmp_its) -> bool {
								return ((my_its == cmp_its) && ...);
							},
							cmp._iter_tup);
				},
				this->_iter_tup);
	}
};
static_assert(std::forward_iterator<
		vert_forward_iterator<typename std::vector<int>::const_iterator>>);

} // namespace detail

template <class First, class... Args>
	requires detail::vertical_iters<First, Args...>
struct vertical_iterator : detail::vert_forward_iterator<First, Args...> {

	using base_t = detail::vert_forward_iterator<First, Args...>;
	using base_t::base_t;

	using difference_type = typename base_t::difference_type;
	using value_type = typename base_t::value_type;
	using pointer = typename base_t::pointer;
	using reference = typename base_t::reference;
	using iterator_category = typename base_t::iterator_category;
	using iterator_concept = typename base_t::iterator_concept;
};

// User CTAD
template <class First, class... Args>
vertical_iterator(First f, Args... args) -> vertical_iterator<First, Args...>;


template <class First, class... Args>
	requires detail::vertical_ranges<First, Args...>
auto vbegin(First& first_container, Args&... containers) {
	// Checks containers are the same size.
	using std::begin;
	using std::end;

	auto s = std::distance(begin(first_container), end(first_container));
	if (!((s == std::distance(begin(containers), end(containers))) && ...)) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Container size mismatch.");
	}

	return vertical_iterator{ begin(first_container), begin(containers)... };

	// if constexpr (std::is_const_v<First>) {
	//	return vertical_iterator{ std::begin(first_container),
	// std::begin(containers)... }; } else { 	static_assert(false);
	//	// return vertical_iterator{ std::begin(first),
	//	//	std::begin(args)... };
	// }
}

template <class First, class... Args>
auto vcbegin(const First& f, const Args&... args) {
	return vbegin(f, args...);
}

template <class First, class... Args>
	requires detail::vertical_ranges<First, Args...>
auto vend(First& first_container, Args&... containers) {
	// Checks containers are the same size.
	// Release builds throw is in fea::vbegin() function.
	using std::begin;
	using std::end;

	assert(((std::distance(begin(first_container), end(first_container))
					== std::distance(begin(containers), end(containers)))
			&& ...));

	return vertical_iterator{ end(first_container), end(containers)... };

	// if constexpr (std::is_const_v<First>) {
	//	return vertical_iterator{ std::end(first_container),
	// std::end(containers)... }; } else { 	static_assert(false);
	//	// return vertical_iterator{ std::end(first), std::end(args)... };
	// }
}

template <class First, class... Args>
auto vcend(const First& f, const Args&... args) {
	return vend(f, args...);
}

} // namespace fea


namespace {
#define FAIL_MSG "vertical_iterators.cpp : failed test"

TEST(vertical_iterators, basics) {
#if 0
	// Check mismatched sizes death.
	{
		const std::vector<int> i_vec(10);
		const std::vector<float> d_vec(9);
		const std::vector<bool> b_vec(8);

#if FEA_DEBUG
		EXPECT_DEATH(fea::vbegin(i_vec, d_vec, b_vec), "");
		EXPECT_DEATH(fea::vend(i_vec, d_vec, b_vec), "");
		// EXPECT_DEATH(fea::vertical_view(i_vec, d_vec, b_vec), "");
#else
		EXPECT_THROW(fea::vbegin(i_vec, d_vec, b_vec), std::invalid_argument);
		// EXPECT_THROW(fea::vend(i_vec, d_vec, b_vec), std::invalid_argument);
		// EXPECT_THROW(
		//		fea::vertical_view(i_vec, d_vec, b_vec), std::invalid_argument);
#endif
	}

	// Check basics, values, deref, member aliases.
	{
		const std::vector<int> i_vec(10);
		const std::vector<float> d_vec(10);
		const std::vector<bool> b_vec(10);

		// Implementation defined.
		using bool_iter_t = typename std::vector<bool>::const_iterator;
		using bool_ref_t = typename bool_iter_t::reference;
		using bool_ptr_t = typename bool_iter_t::pointer;

		auto it = fea::vbegin(i_vec, d_vec, b_vec);
		using vertical_t = std::decay_t<decltype(it)>;
		using value_t = std::decay_t<decltype(*it)>;

		// Check types.
		static_assert(std::is_same_v<typename vertical_t::value_type,
							  std::tuple<const int&, const float&, bool_ref_t>>,
				FAIL_MSG);
		static_assert(std::is_same_v<typename vertical_t::pointer,
							  std::tuple<const int*, const float*, bool_ptr_t>>,
				FAIL_MSG);
		static_assert(std::is_same_v<value_t,
							  std::tuple<const int&, const float&, bool_ref_t>>,
				FAIL_MSG);

		auto it2 = fea::vbegin(i_vec, d_vec, b_vec);
		EXPECT_TRUE(it == it2);
		EXPECT_FALSE(it == ++it2);


		// TODO :
		//*it2 = std::tuple{ 42, 42.0, true };

		// fea::vbegin(i_vec, d_vec, b_vec);
		// fea::vend(i_vec, d_vec, b_vec);
		// std::sort(vit_begin, vit_end,
		//	[](const std::tuple<int&, double&, bool&> lhs,
		//			const std::tuple<int, double, bool>& rhs) {
		//		if (std::get<0>(lhs) == std::get<0>(rhs)) {
		//			return /*compare doubles*/;
		//		}
		//		return /*compare ints*/;
		//	});
	}
#endif
}


// template <class First, class... Args>
//	requires detail::vertical_ranges<First, Args...>
// struct vertical_view {
//	vertical_view(First& first, Args&... args)
//			: _container_tup(first, args...) {
//		// Checks containers are the same size.
//		// Release build throw is in fea::vbegin() function.
//		assert(((std::distance(first.begin(), first.end())
//						== std::distance(args.begin(), args.end()))
//				&& ...));
//	}
//
//	auto begin() const {
//		return std::apply(
//				[](const auto&... args) { return fea::vbegin(args...); },
//				_container_tup);
//	}
//	auto cbegin() const {
//		return std::apply(
//				[](const auto&... args) { return fea::vcbegin(args...); },
//				_container_tup);
//	}
//	auto begin() {
//		return std::apply([](auto&... args) { return fea::vbegin(args...); },
//				_container_tup);
//	}
//
//	auto end() const {
//		return std::apply(
//				[](const auto&... args) { return fea::vend(args...); },
//				_container_tup);
//	}
//	auto cend() const {
//		return std::apply(
//				[](const auto&... args) { return fea::vcend(args...); },
//				_container_tup);
//	}
//	auto end() {
//		return std::apply([](auto&... args) { return fea::vend(args...); },
//				_container_tup);
//	}
//
// private:
//	std::tuple<First&, Args&...> _container_tup;
// };

TEST(vertical_iterators, vertical_view) {
	// maybe a good idea?

	// Shouldn't compile.
	//{
	//	int i;
	//	double d;
	//	bool b;
	//	fea::vertical_view{ i, d, b };
	//}

	// Shouldn't compile.
	//{
	//	std::vector<int> i_vec;
	//	std::unordered_set<int> i_uno;
	//	fea::vertical_view{ i_vec, i_uno };
	//}

	//	{
	//		std::vector<int> i_vec(10);
	//		std::vector<double> d_vec(9);
	//		std::vector<bool> b_vec(8);
	// #if FEA_DEBUG
	//		EXPECT_DEATH(fea::vertical_view(i_vec, d_vec, b_vec), "");
	// #else
	//		EXPECT_THROW(
	//				fea::vertical_view(i_vec, d_vec, b_vec),
	// std::invalid_argument); #endif
	//	}

	// auto view = fea::vertical_view{ i_vec, d_vec, b_vec };
	// std::sort(view.begin(), view.end());
}
} // namespace

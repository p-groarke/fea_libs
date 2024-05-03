#pragma once
namespace fea {
namespace detail {
template <class Enum, size_t N, class Funcs, Enum... Es>
struct switcher;

template <class Enum, size_t N, class... Funcs, Enum... Es>
struct switcher<Enum, N, std::tuple<Funcs...>, Es...> {

	constexpr switcher() = default;
	constexpr switcher(std::tuple<Funcs...>&& funcs)
			: _funcs(std::move(funcs)) {
	}

	template <Enum E, class NewFunc>
	constexpr auto case_(NewFunc&& func) const&& {
		return switcher<Enum, N, std::tuple<Funcs..., NewFunc>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()),
					std::make_tuple(std::forward<NewFunc>(func)))
		};
	}

	template <Enum E>
	constexpr auto empty() const&& {
		auto l = []() {};
		return switcher<Enum, N, std::tuple<Funcs..., decltype(l)>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()), std::make_tuple(l))
		};
	}

	constexpr void operator()(Enum e) const {
		static_assert(
				sizeof...(Es) == N, "safe_switch : missing case statement");

#if FEA_DEBUG
		constexpr std::array<Enum, N> arr{ Es... };
		auto it = std::find(arr.begin(), arr.end(), e);
		assert(it != arr.end());
#endif

		bool found = false;
		fea::static_for<N>([&, this](auto e_idx) {
			if (found) {
				return;
			}

			if (e_idx == size_t(e)) {
				found = true;
				return _funcs.template find<Enum(e_idx())>()();
			}
		});
	}


private:
	fea::type_map<fea::pack_nt<Es...>, Funcs...> _funcs;
};
} // namespace detail

template <class Enum, size_t N>
constexpr auto safe_switch() {
	return detail::switcher<Enum, N, std::tuple<>>{};
}
} // namespace fea
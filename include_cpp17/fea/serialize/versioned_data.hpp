/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "fea/utils/platform.hpp"
#if FEA_VSYEAR > 2017
#include "fea/enum/utility.hpp"
#include "fea/meta/static_for.hpp"

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>


/*
versioned_data is a system that allows you to upgrade serialized data and load
old file types.

It enforces (as much as possible) keeping old version structs around and
enforces upgrade functions between [ver - 1, ver]. Serializing your data using
this scheme, you can always upgrade files to the latest version.
Note, versioned_data only upgrades data forward (old to new).

Your data classes are freeform and can hold whatever they want. They only need
to be templated to a version enum. It is recommended to use reasonable default
values as well. You must serialize the version enum to file of course.

See unit tests for examples. It looks complicated but it really isn't.
*/


namespace fea {
namespace detail {
template <template <auto> class DataT, auto FromVer, auto ToVer>
void upgrade(const DataT<FromVer>&, DataT<ToVer>&);
} // namespace detail

template <class VersionEnum, class...>
struct versioned_data;

template <class VersionEnum, template <VersionEnum> class DataT,
		VersionEnum... VEnums>
struct versioned_data<VersionEnum, DataT<VEnums>...> {
	using version_t = VersionEnum;
	static constexpr VersionEnum latest
			= VersionEnum(fea::to_underlying(VersionEnum::count) - 1);

	static constexpr size_t size = sizeof...(VEnums);
	static constexpr std::array<VersionEnum, size> versions{ VEnums... };
	inline static std::tuple<DataT<VEnums>...> data{};

	// Could static assert that ToVer == latest, but it might prevent
	// some user edge-cases.
	template <VersionEnum FromVer, VersionEnum ToVer>
	void upgrade(const DataT<FromVer>& from, DataT<ToVer>& to) {
		using fea::detail::upgrade;
		upgrade(from, to);
	}

private:
	template <template <auto> class T, auto FromVer, auto ToVer>
	friend void detail::upgrade(const T<FromVer>&, T<ToVer>&);

	static constexpr bool do_asserts() {
		constexpr bool size_ok = size == size_t(VersionEnum::count);
		static_assert(size_ok,
				"fea::versioned_data : VersionEnum must have member 'count', "
				"and it must be equal to the total number of versions.");

		constexpr bool unsigned_ok
				= std::is_unsigned_v<std::underlying_type_t<VersionEnum>>;
		static_assert(unsigned_ok,
				"fea::versioned_data : VersionEnum must be unsigned.");

		using index_seq_t = std::index_sequence<size_t(VEnums)...>;
		using expected_index_seq_t = decltype(std::make_index_sequence<size>{});
		constexpr bool ordered_ok
				= std::is_same_v<index_seq_t, expected_index_seq_t>;
		static_assert(ordered_ok,
				"fea::versioned_data : Data structs must be ordered in the "
				"same order as version enum (start at v0 up to vN).");

		return size_ok & unsigned_ok & ordered_ok;
	}

	static_assert(do_asserts(),
			"fea::versioned_data : Failed to create versioned_data.");
};

namespace detail {
template <class VersionEnum, template <VersionEnum> class DataT,
		size_t... Idxes>
constexpr auto make_versioned_data(std::index_sequence<Idxes...>) {
	return fea::versioned_data<VersionEnum, DataT<VersionEnum(Idxes)>...>{};
}

template <class VersionEnum, template <VersionEnum> class DataT, size_t N>
using versioned_data_t = decltype(make_versioned_data<VersionEnum, DataT>(
		std::make_index_sequence<N>{}));

template <template <auto> class DataT, auto FromVer, auto ToVer>
void upgrade(const DataT<FromVer>& from, DataT<ToVer>& to) {
	using VersionEnum = std::decay_t<decltype(FromVer)>;
	using map_t = detail::versioned_data_t<VersionEnum, DataT,
			size_t(VersionEnum::count)>;
	using tup_t = std::decay_t<decltype(map_t::data)>;

	static_assert(size_t(FromVer) <= size_t(ToVer),
			"fea::versioned_data : Only supports upgrading data in one "
			"direction, old to new.");
	static_assert(size_t(ToVer) - size_t(FromVer) > 1,
			"fea::versioned_data : This overload shouldn't have been "
			"called. You are probably missing an upgrade() "
			"function for a combination of versions.");

	if constexpr (FromVer == ToVer) {
		to = from;
	} else {
		// Initialize our objects we'll use to hold intermediate
		// conversions. std::tuple<DataT<v0>, DataT<v1>, ...>
		tup_t converted_datas{};

		// Prime the starting version data.
		std::get<DataT<FromVer>>(converted_datas) = from;

		// Loop on FromVer to ToVer. Subsequently call upgrade functions one
		// at a time.
		static constexpr size_t msize = size_t(ToVer) - size_t(FromVer);
		fea::static_for<msize>([&](auto const_i) {
			static constexpr size_t i = const_i + size_t(FromVer);
			constexpr VersionEnum from_ver = VersionEnum(i);
			constexpr VersionEnum to_ver = VersionEnum(i + 1);
			static_assert(!std::is_same_v<DataT<from_ver>, DataT<to_ver>>,
					"fea::versioned_data : 2 version datas are exactly the "
					"same. This happens if you forgot to backup an old "
					"version struct.");

			// Get the next version and call the upgrade function.
			const DataT<from_ver>& from
					= std::get<DataT<from_ver>>(converted_datas);
			DataT<to_ver>& to = std::get<DataT<to_ver>>(converted_datas);
			upgrade(from, to);
		});

		to = std::get<DataT<ToVer>>(converted_datas);
	}
}
} // namespace detail
} // namespace fea
#endif

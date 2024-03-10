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
#include "fea/enum/utility.hpp"
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"

#include <array>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>


/*
versioned_data is a system that allows you to upgrade serialized data and load
old file types.

It enforces (as much as possible) keeping old version structs around and
enforces upgrade/downgrade functions between versions. Serializing your data
using this scheme, you can always upgrade files to the latest version.

Your data classes are freeform and can hold whatever they want. They only need
static constexpr (unsigned integer type) version. Version must start at 0 and
grow consecutively.

If you do not call one of upgrade, downgrade, deserialize, you do not need to
have those functions declared.

See unit tests for examples. It looks complicated but it really isn't for the
caller.
*/


namespace fea {
namespace detail {
// Should never be called. These are here only to trigger ADL.
template <class FromT, class ToT>
std::enable_if_t<(ToT::version - FromT::version > 1)> upgrade(
		const FromT&, ToT&);
template <class FromT, class ToT>
std::enable_if_t<(FromT::version - ToT::version > 1)> downgrade(
		const FromT&, ToT&);
template <class D, class T>
std::enable_if_t<T::version
		== (std::numeric_limits<decltype(T::version)>::max)()>
deserialize(D&, T&);

template <class FromT, class ToT>
using mhas_upgrade
		= decltype(upgrade(std::declval<const FromT&>(), std::declval<ToT&>()));
template <class FromT, class ToT>
using mhas_downgrade = decltype(downgrade(
		std::declval<const FromT&>(), std::declval<ToT&>()));
template <class DeserializerT, class ToT>
using mhas_deserialize = decltype(deserialize(
		std::declval<DeserializerT&>(), std::declval<ToT&>()));

template <class, class...>
struct upgrade_traits;

template <class D, class T>
struct upgrade_traits<D, T> {
	static constexpr bool has_upgrade = true;
	static constexpr bool has_downgrade = true;
	static constexpr bool has_deserialize
			= fea::is_detected_v<mhas_deserialize, D, T>;
};

template <class DeserializerT, class T, class U, class... DataTs>
struct upgrade_traits<DeserializerT, T, U, DataTs...> {
	static constexpr bool has_upgrade = fea::is_detected_v<mhas_upgrade, T, U>
			&& upgrade_traits<DeserializerT, U, DataTs...>::has_upgrade;
	static constexpr bool has_downgrade
			= fea::is_detected_v<mhas_downgrade, U, T>
			&& upgrade_traits<DeserializerT, U, DataTs...>::has_downgrade;
	static constexpr bool has_deserialize
			= fea::is_detected_v<mhas_deserialize, DeserializerT, T>
			&& upgrade_traits<DeserializerT, U, DataTs...>::has_deserialize;
};

template <class D, class... DataTs>
inline constexpr bool all_have_upgrade_v
		= upgrade_traits<D, DataTs...>::has_upgrade;
template <class D, class... DataTs>
inline constexpr bool all_have_downgrade_v
		= upgrade_traits<D, DataTs...>::has_downgrade;
template <class D, class... DataTs>
inline constexpr bool all_have_deserialize_v
		= upgrade_traits<D, DataTs...>::has_deserialize;
} // namespace detail

template <class... DataTs>
struct versioned_data {
	// My version type.
	using version_t = std::decay_t<decltype(fea::front_t<DataTs...>::version)>;
	// Latest version.
	static constexpr version_t latest = fea::back_t<DataTs...>::version;
	// Latest version data type.
	using latest_data_t = fea::back_t<DataTs...>;
	// Total number of data structs.
	static constexpr size_t size = sizeof...(DataTs);
	// All versions in an array.
	static constexpr std::array<version_t, size> versions{ DataTs::version... };
	// Tuple of all consecutive data structs.
	using data_tup_t = std::tuple<DataTs...>;

	// Call this to upgrade an old version to a newer version.
	// Pass the old data struct and the new output data struct.
	// This does some checks to make sure you haven't forgotten anything.
	template <class FromT, class ToT>
	static void upgrade(const FromT& from, ToT& to) {
		static_assert(FromT::version <= ToT::version,
				"fea::versioned_data : Upgrade only supports upgrading data in "
				"one direction, old to new.");

		static_assert(detail::all_have_upgrade_v<void, DataTs...>,
				"fea::versioned_data : One or more of the data types do not "
				"have an upgrade function.\n");

		if constexpr (FromT::version == ToT::version) {
			// Nothing to do.
			to = from;
		} else {
			// Initialize our objects we'll use to hold intermediate
			// conversions. std::tuple<DataT<v0>, DataT<v1>, ...>
			data_tup_t converted_datas{};

			// Prime the starting version data.
			std::get<FromT>(converted_datas) = from;

			// Loop on FromVer to ToVer. Subsequently call upgrade functions one
			// at a time.
			static constexpr size_t msize
					= size_t(ToT::version) - size_t(FromT::version);
			fea::static_for<msize>([&](auto const_i) {
				static constexpr size_t i = const_i + size_t(FromT::version);
				constexpr size_t from_idx = i;
				constexpr size_t to_idx = i + 1;
				using mfrom_t = std::tuple_element_t<from_idx, data_tup_t>;
				using mto_t = std::tuple_element_t<to_idx, data_tup_t>;

				static_assert(!std::is_same_v<mfrom_t, mto_t>,
						"fea::versioned_data : 2 version datas are exactly the "
						"same. This happens if you forgot to backup an old "
						"version struct.");

				// Get the next version and call the upgrade function.
				const auto& from = std::get<from_idx>(converted_datas);
				auto& to = std::get<to_idx>(converted_datas);
				using fea::detail::upgrade;
				upgrade(from, to);
			});

			to = std::get<ToT>(converted_datas);
		}
	}

	// Call this to downgrade from a new version to an old version.
	// Pass the new data struct and the old output data struct.
	// This does some checks to make sure you haven't forgotten anything.
	template <class FromT, class ToT>
	static void downgrade(const FromT& from, ToT& to) {
		static_assert(FromT::version >= ToT::version,
				"fea::versioned_data : Downgrade only supports downgrading "
				"data in one direction, new to old.");

		static_assert(detail::all_have_downgrade_v<void, DataTs...>,
				"fea::versioned_data : One or more of the data types do not "
				"have a downgrade function.\n");

		if constexpr (FromT::version == ToT::version) {
			// Nothing to do.
			to = from;
		} else {
			// Initialize our objects we'll use to hold intermediate
			// conversions. std::tuple<DataT<v0>, DataT<v1>, ...>
			data_tup_t converted_datas{};

			// Prime the starting version data.
			std::get<FromT>(converted_datas) = from;

			// Loop on FromVer to ToVer. Subsequently call upgrade functions one
			// at a time.
			static constexpr size_t msize
					= size_t(FromT::version) - size_t(ToT::version);
			fea::static_for_reversed<msize>([&](auto const_i) {
				static constexpr size_t i = const_i + size_t(ToT::version);
				constexpr size_t from_idx = i + 1;
				constexpr size_t to_idx = i;
				using mfrom_t = std::tuple_element_t<from_idx, data_tup_t>;
				using mto_t = std::tuple_element_t<to_idx, data_tup_t>;

				static_assert(!std::is_same_v<mfrom_t, mto_t>,
						"fea::versioned_data : 2 version datas are exactly the "
						"same. This happens if you forgot to backup an old "
						"version struct.");

				// Get the next version and call the upgrade function.
				const auto& from = std::get<from_idx>(converted_datas);
				auto& to = std::get<to_idx>(converted_datas);
				using fea::detail::downgrade;
				downgrade(from, to);
			});

			to = std::get<ToT>(converted_datas);
		}
	}

	// Call this function with a data version to trigger deserialization and
	// upgrade.
	// In your serialized data, always store a version at the very beginning.
	// This will allow you to use this all without knowing (or caring) about the
	// actual data version.
	// As always, you need 1 deserialize function per version.
	template <class DeserializerT>
	static void deserialize(version_t version, DeserializerT& your_deserializer,
			latest_data_t& latest_data) {
		static_assert(detail::all_have_deserialize_v<DeserializerT, DataTs...>,
				"fea::versioned_data : One or more of the data types do not "
				"have a deserialize function.\n");

		if (version == latest) {
			using fea::detail::deserialize;
			deserialize(your_deserializer, latest_data);
			return;
		}

		// Find the old version.
		fea::static_for<size>([&](auto const_i) {
			constexpr size_t i = const_i;
			if (i != version) {
				return;
			}

			// Deserializer to old data type.
			using old_data_t = std::tuple_element_t<i, data_tup_t>;
			old_data_t from{};
			using fea::detail::deserialize;
			deserialize(your_deserializer, from);

			// Then upgrade.
			versioned_data::upgrade(from, latest_data);
		});
	}

private:
	// Checks and enforcements.
	static constexpr bool do_asserts() {
		using mversion_t
				= std::decay_t<decltype(fea::front_t<DataTs...>::version)>;

		constexpr bool unsigned_ok = std::is_unsigned_v<mversion_t>;
		static_assert(
				unsigned_ok, "fea::versioned_data : Version must be unsigned.");

		constexpr version_t first = fea::front_t<DataTs...>::version;
		constexpr bool first_zero = first == 0;
		static_assert(first_zero,
				"fea::versioned_data : First version must be zero.");

		using index_seq_t = std::index_sequence<size_t(DataTs::version)...>;
		using expected_seq_t
				= decltype(std::make_index_sequence<sizeof...(DataTs)>{});
		constexpr bool ordered_ok = std::is_same_v<index_seq_t, expected_seq_t>;
		static_assert(ordered_ok,
				"fea::versioned_data : Data structs must be ordered in the "
				"same order as their version, starting at 0 to N.");

		return unsigned_ok & first_zero & ordered_ok;
	}

	static_assert(do_asserts(),
			"fea::versioned_data : Failed to create versioned_data.");
};
} // namespace fea

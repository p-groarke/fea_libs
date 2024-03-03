#include <algorithm>
#include <chrono>
#include <fea/performance/tls.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

#if FEA_WITH_TBB
#include <tbb/parallel_for.h>
#endif

namespace {
#if FEA_DEBUG || FEA_NOTHROW
#define FEA_EXPECT_THROW(t, e) EXPECT_DEATH(t, "")
#else
#define FEA_EXPECT_THROW(t, e) EXPECT_THROW(t, e)
#endif

void fuzzit(size_t num_fuzz) {
	// Makes sure everything gets created right.
	{
		fea::tls<int> tls;
		EXPECT_TRUE(tls.empty());
		EXPECT_EQ(tls.size(), 0u);
		{
			size_t num = 0;
			tls.combine_each([&](int) { ++num; });
			EXPECT_EQ(num, 0u);
		}

		std::vector<std::thread::id> tids(num_fuzz);
		std::vector<std::thread> threads;
		threads.reserve(num_fuzz);

		for (size_t i = 0; i < num_fuzz; ++i) {
			threads.push_back(std::thread{ [&tls, &tids, idx = i]() {
				assert(idx < tids.size());
				tids[idx] = std::this_thread::get_id();
				fea::tls_lock<int> lock = tls.lock();
				lock.local() = 42;
			} });
		}

		for (std::thread& t : threads) {
			t.join();
		}

		std::sort(tids.begin(), tids.end());
		auto new_end = std::unique(tids.begin(), tids.end());
		tids.erase(new_end, tids.end());

		EXPECT_EQ(tids.size(), tls.size());

		{
			size_t num = 0;
			tls.combine_each([&](int v) {
				EXPECT_EQ(v, 42);
				++num;
			});
			EXPECT_EQ(tls.size(), num);
		}
	}

#if FEA_WITH_TBB
	{
		fea::tls<std::vector<int>> tls;

		auto tbb_fuzz = [&](const tbb::blocked_range<size_t>& range) {
			fea::tls_lock<std::vector<int>> lock{ tls };
			std::vector<int>& v = lock.local();

			for (size_t i = range.begin(); i < range.end(); ++i) {
				v.push_back(int(i));
			}
		};

		auto outer_fuzz = [&](const tbb::blocked_range<size_t>& range) {
			fea::tls_lock<std::vector<int>> lock{ tls };
			const std::vector<int>& v = lock.local();
			size_t backup_size = v.size();

			for (size_t i = range.begin(); i < range.end(); ++i) {
				tbb::parallel_for(tbb::blocked_range<size_t>{ 0, num_fuzz, 1 },
						tbb_fuzz, tbb::static_partitioner{});
			}
			EXPECT_EQ(v.size(), backup_size);
		};
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, num_fuzz, 1 },
				outer_fuzz, tbb::static_partitioner{});

		std::vector<int> results;
		tls.combine_each([&](const std::vector<int>& v) {
			results.insert(results.end(), v.begin(), v.end());
		});

		// At this point, we should have num_fuzz * num_fuzz ints.
		// And recursion should have kicked in.
		std::vector<int> counts(num_fuzz);
		for (int r : results) {
			++counts[r];
		}

		for (int c : counts) {
			EXPECT_EQ(c, int(num_fuzz));
		}

		tls.clear();
		EXPECT_EQ(tls.size(), 0u);
		EXPECT_TRUE(tls.empty());
		{
			size_t num = 0;
			tls.combine_each([&](auto&) { ++num; });
			EXPECT_EQ(num, 0u);
		}

		// Again for kicks.
		tbb::parallel_for(tbb::blocked_range<size_t>{ 0, num_fuzz, 1 },
				outer_fuzz, tbb::static_partitioner{});

		results.clear();
		tls.combine_each([&](const std::vector<int>& v) {
			results.insert(results.end(), v.begin(), v.end());
		});
		counts.clear();
		counts.resize(num_fuzz);
		for (int r : results) {
			++counts[r];
		}
		for (int c : counts) {
			EXPECT_EQ(c, int(num_fuzz));
		}

		tls.clear();
		EXPECT_EQ(tls.size(), 0u);
		EXPECT_TRUE(tls.empty());
		{
			size_t num = 0;
			tls.combine_each([&](auto&) { ++num; });
			EXPECT_EQ(num, 0u);
		}
	}
#endif
}

TEST(tls, basics) {
	fea::tls<int> tls;

	{
		fea::tls_lock<int> lock = tls.lock();
		int& thread_vec = lock.local();
		thread_vec = 42;
	}

	EXPECT_EQ(tls.size(), 1u);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](const int& v) {
			EXPECT_EQ(v, 42);
			++num;
		});
		EXPECT_EQ(num, 1u);
	}

	tls.clear();
	EXPECT_EQ(tls.size(), 0u);
	EXPECT_TRUE(tls.empty());

	{
		fea::tls_lock<int> lock{ tls };
		int& thread_vec = lock.local();
		thread_vec = 42;
	}

	EXPECT_EQ(tls.size(), 1u);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](const int& v) {
			EXPECT_EQ(v, 42);
			++num;
		});
		EXPECT_EQ(num, 1u);
	}

	tls.clear();
	EXPECT_EQ(tls.size(), 0u);
	EXPECT_TRUE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](int) { ++num; });
		EXPECT_EQ(num, 0u);
	}

	tls.clear();
	{
		EXPECT_EQ(tls.size(), 0u);
		std::vector<fea::tls_lock<int>> locks;
		locks.push_back(tls.lock());
		EXPECT_EQ(tls.size(), 1u);

		EXPECT_NO_THROW(locks.push_back(tls.lock()));
		EXPECT_EQ(tls.size(), 2u);

		EXPECT_NO_THROW(locks.push_back(tls.lock()));
		EXPECT_EQ(tls.size(), 3u);

		EXPECT_NO_THROW(locks.push_back(tls.lock()));
		EXPECT_EQ(tls.size(), 4u);

		FEA_EXPECT_THROW(tls.clear(), std::runtime_error);
		FEA_EXPECT_THROW(tls.combine_each([](int) {}), std::runtime_error);
	}
	EXPECT_EQ(tls.size(), 4u);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](int) { ++num; });
		EXPECT_EQ(num, tls.size());
	}


	tls.clear();
	{
		EXPECT_EQ(tls.size(), 0u);
		std::vector<fea::tls_lock<int>> locks;
		locks.push_back(fea::tls_lock<int>{ tls });
		EXPECT_EQ(tls.size(), 1u);

		EXPECT_NO_THROW(locks.push_back(fea::tls_lock<int>{ tls }));
		EXPECT_EQ(tls.size(), 2u);

		EXPECT_NO_THROW(locks.push_back(fea::tls_lock<int>{ tls }));
		EXPECT_EQ(tls.size(), 3u);

		EXPECT_NO_THROW(locks.push_back(fea::tls_lock<int>{ tls }));
		EXPECT_EQ(tls.size(), 4u);

		EXPECT_NO_THROW(locks.push_back(fea::tls_lock<int>{ tls }));
		EXPECT_EQ(tls.size(), 5u);

		EXPECT_NO_THROW(locks.push_back(fea::tls_lock<int>{ tls }));
		EXPECT_EQ(tls.size(), 6u);

		FEA_EXPECT_THROW(tls.clear(), std::runtime_error);
		FEA_EXPECT_THROW(tls.combine_each([](int) {}), std::runtime_error);
	}
	EXPECT_EQ(tls.size(), 6u);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](int) { ++num; });
		EXPECT_EQ(num, tls.size());
	}


	tls.clear();
	{
		std::vector<std::thread::id> tids(4);
		std::thread t1{ [&]() {
			tids[0] = std::this_thread::get_id();
			fea::tls_lock<int> lock = tls.lock();
		} };
		std::thread t2{ [&]() {
			tids[1] = std::this_thread::get_id();
			fea::tls_lock<int> lock = tls.lock();
		} };
		std::thread t3{ [&]() {
			tids[2] = std::this_thread::get_id();
			fea::tls_lock<int> lock = tls.lock();
		} };
		std::thread t4{ [&]() {
			tids[3] = std::this_thread::get_id();
			fea::tls_lock<int> lock = tls.lock();
		} };

		t1.join();
		t2.join();
		t3.join();
		t4.join();

		std::sort(tids.begin(), tids.end());
		auto new_end = std::unique(tids.begin(), tids.end());
		tids.erase(new_end, tids.end());
		EXPECT_EQ(tids.size(), tls.size());
	}

	fuzzit(100);
	fuzzit(1'000);
}
} // namespace
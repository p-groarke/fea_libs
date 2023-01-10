#include <algorithm>
#include <fea/performance/tls.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
#if FEA_DEBUG || defined(FEA_NOTHROW)
#define FEA_EXPECT_THROW(t, e) EXPECT_DEATH(t, "")
#else
#define FEA_EXPECT_THROW(t, e) EXPECT_THROW(t, e)
#endif

template <class T>
void fuzzit(size_t num_fuzz, fea::tls<T>& tls) {
	tls.clear();
	EXPECT_TRUE(tls.empty());
	EXPECT_EQ(tls.size(), 0);
	{
		size_t num = 0;
		tls.combine_each([&](int) { ++num; });
		EXPECT_EQ(num, 0);
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

TEST(tls, basics) {
	fea::tls<int> tls;

	{
		fea::tls_lock<int> lock = tls.lock();
		int& thread_vec = lock.local();
		thread_vec = 42;
	}

	EXPECT_EQ(tls.size(), 1);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](const int& v) {
			EXPECT_EQ(v, 42);
			++num;
		});
		EXPECT_EQ(num, 1);
	}

	tls.clear();
	EXPECT_EQ(tls.size(), 0);
	EXPECT_TRUE(tls.empty());

	{
		fea::tls_lock<int> lock{ tls };
		int& thread_vec = lock.local();
		thread_vec = 42;
	}

	EXPECT_EQ(tls.size(), 1);
	EXPECT_FALSE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](const int& v) {
			EXPECT_EQ(v, 42);
			++num;
		});
		EXPECT_EQ(num, 1);
	}

	tls.clear();
	EXPECT_EQ(tls.size(), 0);
	EXPECT_TRUE(tls.empty());
	{
		size_t num = 0;
		tls.combine_each([&](int) { ++num; });
		EXPECT_EQ(num, 0);
	}

	{
		fea::tls_lock<int> lock = tls.lock();
		FEA_EXPECT_THROW(auto t = tls.lock(), std::runtime_error);
		FEA_EXPECT_THROW(auto t = tls.lock(), std::runtime_error);
		FEA_EXPECT_THROW(auto t = tls.lock(), std::runtime_error);
		FEA_EXPECT_THROW(tls.clear(), std::runtime_error);
		FEA_EXPECT_THROW(tls.combine_each([](int) {}), std::runtime_error);
	}

	{
		fea::tls_lock<int> lock{ tls };
		FEA_EXPECT_THROW(fea::tls_lock<int>{ tls }, std::runtime_error);
		FEA_EXPECT_THROW(fea::tls_lock<int>{ tls }, std::runtime_error);
		FEA_EXPECT_THROW(fea::tls_lock<int>{ tls }, std::runtime_error);
		FEA_EXPECT_THROW(tls.clear(), std::runtime_error);
		FEA_EXPECT_THROW(tls.combine_each([](int) {}), std::runtime_error);
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

	fuzzit(100, tls);
	fuzzit(1'000, tls);

	tls.clear();
	{
		fea::tls_lock<int> lock{ tls };
		std::thread t1{ [&]() {
			// EXPECT_DEATH(lock.local(), "");
			EXPECT_DEATH(fea::tls_lock<int>{ std::move(lock) }, "");
		} };
	}

	{}
}
} // namespace
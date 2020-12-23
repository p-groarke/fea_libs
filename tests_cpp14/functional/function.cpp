#include <algorithm>
#include <fea/benchmark/benchmark.hpp>
#include <fea/functional/function.hpp>
#include <gtest/gtest.h>
#include <random>

namespace {
struct bench_obj {
	void func(size_t& i) {
		++i;
	}
};

TEST(raw_function, benchmark) {
	constexpr size_t bench_count = 100'000'000;

	std::random_device rnd_device;
	std::mt19937_64 engine{ rnd_device() };
	std::uniform_int_distribution<size_t> dist{ 1, 5 };

	std::vector<size_t> vec(10'000'000);
	std::generate(vec.begin(), vec.end(),
			[&dist, &engine]() { return dist(engine); });

	bench_obj obj;
	size_t answer = 0;

	void (bench_obj::*raw_ptr)(size_t&) = &bench_obj::func;
	std::function<void(bench_obj*, size_t&)> std_function = &bench_obj::func;
	fea::raw_function<void(bench_obj*, size_t&)> fea_raw_function
			= &bench_obj::func;
	fea::raw_function_with_if<void(bench_obj*, size_t&)>
			fea_raw_function_with_if = &bench_obj::func;

	fea::bench::suite suite;
	suite.average(10);
	suite.benchmark("Raw Member Pointer", [&]() {
		for (size_t& n : vec) {
			(obj.*raw_ptr)(n);
			// answer += n;
		}
	});
	suite.benchmark("std::function", [&]() {
		for (size_t& n : vec) {
			std_function(&obj, n);
			// answer += n;
		}
	});
	suite.benchmark("fea::raw_function", [&]() {
		for (size_t& n : vec) {
			fea_raw_function(&obj, n);
			// answer += n;
		}
	});
	suite.benchmark("fea::raw_function with if", [&]() {
		for (size_t& n : vec) {
			fea_raw_function_with_if(&obj, n);
			// answer += n;
		}
	});
	suite.print();
	suite.clear();


	suite.benchmark("Raw Member Pointer", [&]() {
		for (size_t i = 0; i < bench_count; ++i) {
			(obj.*raw_ptr)(answer);
		}
	});
	suite.benchmark("std::function", [&]() {
		for (size_t i = 0; i < bench_count; ++i) {
			std_function(&obj, answer);
		}
	});
	suite.benchmark("fea::raw_function", [&]() {
		for (size_t i = 0; i < bench_count; ++i) {
			fea_raw_function(&obj, answer);
		}
	});
	suite.benchmark("fea::raw_function with if", [&]() {
		for (size_t i = 0; i < bench_count; ++i) {
			fea_raw_function_with_if(&obj, answer);
		}
	});
	suite.print();

	printf("\n\n%zu\n", answer);
}

void func(int v) {
	printf("%d\n", v);
}

struct potato {
	void func(int v) {
		printf("%d\n", v + my_val);
	}
	int my_val = 42;
};

template <class T, class FuncRet, class... FuncArgs>
void print_ptr(FuncRet (T::*func)(FuncArgs...)) {
	void* ptr = std::addressof(func);
	printf("%s\n", typeid(func).name());
	printf("%s\n", typeid(ptr).name());
}

template <class T, class FuncRet, class... FuncArgs>
void* to_void_ptr(FuncRet (T::*func)(FuncArgs...)) {
	return std::addressof(func);
}
// template <class T, class FuncRet, class FirstArg, class... FuncArgs>
// void call_ptr(T* obj, FuncRet (T::*func)(FirstArg, FuncArgs...)) {
//	void* ptr = std::addressof(func);
//
//
//
//	printf("%s\n", typeid(func).name());
//	printf("%s\n", typeid(ptr).name());
//}

// template <class Func>
// void print_template(Func func) {
//	void* ptr = std::addressof(func);
//	printf("%s\n", typeid(func).name());
//}

// struct blee {
//	using func_t = void (*)(int);
//
//	blee()
//			: f(&func) {
//	}
//
//	void func(int) {
//	}
//
//	func_t f;
//};

TEST(raw_function, basics) {
	using func_t = void (potato::*)(int);
	// using test2 = void (*)(int);

	// test2 t2 = &(&potato::func);

	potato p;
	// p.my_val = -42;
	//[[maybe_unused]] potato p2;
	//[[maybe_unused]] potato p3;
	//[[maybe_unused]] potato p4;
	//[[maybe_unused]] potato p5;
	//[[maybe_unused]] potato p6;

	// constexpr size_t s = sizeof(&potato::func);

	// void* ptr = to_void_ptr(&potato::func);
	// func_t f_ptr = *(static_cast<func_t*>(ptr));
	//(p.*f_ptr)(101);
	//(p2.*f_ptr)(101);
	//(p3.*f_ptr)(101);
	//(p4.*f_ptr)(101);
	//(p5.*f_ptr)(101);
	//(p6.*f_ptr)(101);

	// print_ptr(&potato::func);
	// print_template(&potato::func);

	// auto* ptr = std::addressof(&potato::func);
	//(p.*(t))(42);
	// auto t2 = &potato::func;

	fea::raw_function<void(int)> fun_ok = &func;
	fun_ok(42);

	fea::raw_function<void(potato*, int)> mem_fun = &potato::func;
	mem_fun(&p, 100);
	using test_type =
			typename fea::raw_function<void(potato*, int)>::member_function_t;
	static_assert(std::is_same<test_type, void (potato::*)(int)>::value, "");


	std::function<void(potato*, int)> stdfun = &potato::func;
}

TEST(function, basics) {
	fea::function<void(int)> f;
	static_assert(std::is_same<decltype(f), std::function<void(int)>>::value,
			"function.cpp : test failed");

	fea::function<void(int), true> f_ptr;
	static_assert(
			std::is_same<decltype(f_ptr), fea::raw_function<void(int)>>::value,
			"function.cpp : test failed");
}
} // namespace

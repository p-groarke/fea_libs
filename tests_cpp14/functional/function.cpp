#include <fea/functional/function.hpp>
#include <gtest/gtest.h>

namespace {

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

TEST(raw_function, basics) {
	using func_t = void (potato::*)(int);
	// using test2 = void (*)(int);

	// test2 t2 = &(&potato::func);

	potato p;
	p.my_val = -42;
	[[maybe_unused]] potato p2;
	[[maybe_unused]] potato p3;
	[[maybe_unused]] potato p4;
	[[maybe_unused]] potato p5;
	[[maybe_unused]] potato p6;

	void* ptr = to_void_ptr(&potato::func);
	func_t f_ptr = *(static_cast<func_t*>(ptr));
	(p.*f_ptr)(101);
	(p2.*f_ptr)(101);
	(p3.*f_ptr)(101);
	(p4.*f_ptr)(101);
	(p5.*f_ptr)(101);
	(p6.*f_ptr)(101);

	// print_ptr(&potato::func);
	// print_template(&potato::func);

	// auto* ptr = std::addressof(&potato::func);

	//(p.*(t))(42);

	// auto t2 = &potato::func;

	// fea::raw_function<void(potato*, int)> rf = &potato::func;

	// std::function<void(potato*, int)> stdf = &potato::func;
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

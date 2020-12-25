#include <algorithm>
#include <chrono>
#include <fea/benchmark/benchmark.hpp>
#include <fea/functional/function.hpp>
#include <gtest/gtest.h>

namespace {
struct potato {
	void func(int v) {
		printf("%d\n", v + my_val);
	}
	void const_func(int v) const {
		printf("%d\n", v + my_val);
	}
	int my_val = 42;
};

void func(int v) {
	printf("%d\n", v);
}

void pfunc(potato*, int v) {
	printf("pertato %d\n", v);
}

struct tomato {
	void func_before(int) {
		printf("before splooosions\n");
	}

	void func(int v) {
		printf("%d\n", v - my_val);
	}

	void func_after(int) {
		printf("after splooosions\n");
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

TEST(raw_function_with_erasure, basics) {
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

	fea::function_cl<void(int)> fun_ok = &func;
	fun_ok(42);

	fea::function_cl<void(potato*, int)> mem_fun = &potato::func;
	mem_fun(&p, 100);
	using test_type = typename fea::function_cl<void(potato*, int)>::member_t;
	static_assert(std::is_same<test_type, void (potato::*)(int)>::value, "");

	//{
	//	auto lam = [&](int) { return; };
	//	fea::raw_function<void(int)> lambda_fun(&decltype(lam)::operator());
	//}

	{
		auto lam = [](int) { return; };
		fea::function_cl<void(int)> lambda_fun(lam);
	}

	// std::function<void(potato*, int)> stdfun = &potato::func;
	// std::function<void(int)> stdfun2 = [&](int) {};

	// struct functor {
	//	void operator()(int) const {
	//	}
	//};
	// std::function<void(int)> stdfun3 = functor{};
}

template <class>
struct func_holder;

template <class Ret, class... Args>
struct func_holder<Ret(Args...)> {};

template <class CastTo, class Ret, class T, class... Args>
auto member_cast(Ret (T::*func)(Args...)) {
	using cast_to = Ret (CastTo::*)(Args...);
	return reinterpret_cast<cast_to>(func);
}

template <class CastTo, class Ret, class... Args>
auto free_to_member_cast(Ret (*func)(Args...)) {
	using cast_to = Ret (CastTo::*)(Args...);
	return reinterpret_cast<cast_to>(func);
}

template <class FuncPtr, class Ret, class T, class... Args>
Ret member_func_caller(T* obj, Args... func_args) {
}

template <class Ret, class T, class... Args>
auto member_to_free_func(Ret (T::*func)(Args...)) {
	using test = decltype(func);
	return &member_func_caller<Ret, T, Args...>;
}

// template <class CastTo, class Ret, class... Args>
// auto free_to_member_cast(Ret (*func)(Args...)) {
//	using cast_to = Ret (CastTo::*)(Args...);
//	return reinterpret_cast<cast_to>(func);
//}

// template <class Func, class T>
// void do_func(Func func, T* obj) {
//	(obj->*func)(42);
//}

template <class Ret, class T, class... Args>
void do_func(Ret (T::*func)(Args...), T* obj) {
	using func_t = decltype(func);
	using test = decltype((std::declval<T>().*std::declval<func_t>())(
			std::declval<Args>()...));
	// std::invoke((std::declval<T>().*std::declval<func_t>())(
	//		std::declval<Args>()...));
	(obj->*func)(42);

	constexpr auto l = []() {};
}

// template <class Func, class T>
// void do_func(T* obj) {
//	(obj->*Func)(42);
//}

template <class T>
using has_args_func
		= decltype(std::declval<T>().args_func(std::declval<int&>()));

// template <class FuncName, class T>
// void do_func(FuncName func, T* obj) {
//	(obj->*func)(42);
//}

struct temp_test {
	template <class FuncRet, class T, class... Args>
	temp_test(FuncRet (T::*func)(Args...)) {
		_lambda = []() {};
	}

	using lambda_t = void (*)(int);
	lambda_t _lambda = nullptr;
};

TEST(function, more_experiments) {
	using potato_f = void (potato::*)(int);
	potato_f fun = member_cast<potato>(&tomato::func);

	using test = decltype(&tomato::func);

	tomato t;
	do_func(&tomato::func, &t);
	// do_func(func, &t);
	// do_func<decltype(&tomato::func)>(&t);


	// using t_test = mem_traits_t<decltype(&tomato::func)>;


	// potato_f free_func = free_to_member_cast<potato>(&func);

	auto recast_fun = member_cast<tomato>(fun);
	(t.*recast_fun)(42);
}

struct test_obj {
	void func(int) {
		printf("test_obj::func(int)\n");
	}
	void func_const(int) const {
		printf("test_obj::func(int) const\n");
	}
};

struct other_t {
	using mem_me_t = int other_t::*;
	using some_func_t = void (&)(int);

	other_t()
			: some_func(func) {
	}

	const some_func_t some_func;
};

TEST(function_cl, basics) {
	std::function<void(test_obj*, int)> stdf = &test_obj::func_const;

	fea::function_cl<void(test_obj*, int)> f = &test_obj::func_const;

	test_obj o;
	f(&o, 42);
}

template <size_t N>
struct base {
	static constexpr size_t test = N;
	// virtual std::bool_constant m_type() = 0;
};
// struct derived_false : base {
//	using type = std::false_type;
//};
// struct derived_true : base {
//	using type = std::true_type;
//};


// struct u1 {
//	union {
//		int32_t first = -42;
//		uint32_t second;
//	};
//};
//
// struct u2 {
//	union {
//		uint32_t first = 42;
//		int32_t second;
//	};
//};
//
// struct u_containter : public u1, public u2 {
//	u_containter(bool)
//			: u1() {
//	}
//	u_containter(int)
//			: u2() {
//	}
//};

struct derived : base<42> {};

// struct star_remover {
//	void operator*(auto test) {
//	}
//};

// template <class T, class CFunc>
// CFunc* operator->*(T*, CFunc* f) {
//	return f;
//}
//
// template <class Ret, class T, class... Args>
// auto operator->*(Ret (T::*f)(Args...)) {
//	return [o = static_cast<T*>(obj), f_ptr = f](
//				   Args... args) { (o->*f_ptr)(args...); };
//}
//
// template <class Ret, class T, class... Args>
// auto operator->*(Ret (T::*f)(Args...) const) {
//	return [o = static_cast<T*>(obj), f_ptr = f](
//				   Args... args) { (o->*f_ptr)(args...); };
//}

struct ptr_remover {
	template <class CFunc>
	CFunc* operator->*(CFunc* f) const {
		return f;
	}

	template <class Ret, class T, class... Args>
	auto operator->*(Ret (T::*f)(Args...)) const {
		return [o = static_cast<T*>(obj), f_ptr = f](
					   Args... args) { (o->*f_ptr)(args...); };
	}

	template <class Ret, class T, class... Args>
	auto operator->*(Ret (T::*f)(Args...) const) const {
		return [o = static_cast<T*>(obj), f_ptr = f](
					   Args... args) { (o->*f_ptr)(args...); };
	}

	// star_remover operator->() const {
	//	return {};
	//}
	// star_remover operator->() {
	//	return {};
	//}

	void* obj = nullptr;
};

// template <class Ret, class T, class... Args>
// auto operator->*(T& obj, Ret (*f)(T*, Args...)) {
//	return [o = &obj, f_ptr = f](Args... args) {
//		f_ptr(o, std::forward<decltype(args)>(args)...);
//	};
//}
//
// template <class Ret, class T, class... Args>
// auto operator->*(T& obj, Ret (T::*f)(Args...)) {
//	return [o = &obj, f_ptr = f](Args... args) {
//		(o->*f_ptr)(std::forward<decltype(args)>(args)...);
//	};
//}
//
// template <class Ret, class T, class... Args>
// auto operator->*(T& obj, Ret (T::*f)(Args...) const) {
//	return [o = &obj, f_ptr = f](Args... args) {
//		(o->*f_ptr)(std::forward<decltype(args)>(args)...);
//	};
//}

template <class>
struct mem_traits;

template <class Ret, class... Args>
struct mem_traits<Ret (*)(Args...)> {
	using type = char;
};

template <class Ret, class T, class... Args>
struct mem_traits<Ret (T::*)(Args...)> {
	using type = short;
};

template <class Ret, class T, class... Args>
struct mem_traits<Ret (T::*)(Args...) const> {
	using type = int;
};

template <class... Args>
using mem_traits_t = typename mem_traits<Args...>::type;


template <class F, class... Args>
using call_t = decltype(std::declval<F>()(std::declval<Args>()...));

// template <class F, class... Args>
// using call_t = decltype(std::declval<F>()(std::declval<Args>()...));

template <class F, class... Args>
using is_callable = fea::is_detected<call_t, F, Args...>;

static_assert(fea::is_detected_v<call_t, void (*)(int), int>, "");
static_assert(fea::is_detected_v<call_t, decltype(&func), int>, "");
// static_assert(fea::is_detected_v<call_t, decltype(&potato::func), int>, "");


TEST(function, basics) {
	// std::variant<int, double> v;
	// std::variant<double, int> v2;
	std::variant<std::variant<int, double>, std::variant<double, int>> v;
	v = std::variant<double, int>{ 42 };

	// get an int
	int i = std::get<0>(std::get<0>(v));
	i;

	// mem_traits<decltype(&func)> mem_test;
	// mem_test = mem_traits<decltype(&potato::func)>{};

	static_assert(sizeof(mem_traits_t<decltype(&func)>) == 1, "");
	static_assert(sizeof(mem_traits_t<decltype(&potato::func)>) == 2, "");
	static_assert(sizeof(mem_traits_t<decltype(&potato::const_func)>) == 4, "");

	auto ct = std::mem_fn(&potato::const_func);
	constexpr size_t s = sizeof(ct);

	// using somefunc_t = void (*)(int);
	// somefunc_t ptr = &func;
	// ptr_remover rem;
	//(rem->*ptr)(42);

	// using somepfunc_t = void (*)(potato*, int);
	// somepfunc_t pot_func = &pfunc;

	// using somememfunc_t = void (potato::*)(int);
	// somememfunc_t mem_ptr = &potato::func;


	// potato p;
	//(p->*mem_ptr)(42);

	//(p->*pot_func)(42);

	// rem.obj = &p;
	//(rem->*mem_ptr)(42);

	// size_t s = offsetof(potato, func);

	// somefunc_t test_ptr = (p.*mem_ptr);

	// base b;
	// b = derived{};
	// printf("%zu\n", b.test);


	// u_containter c(true);
	// std::cout << c.first << std::endl;
	// templ t{ 42 };

	// base b = derived_false{};
	// static_assert(std::is_same<decltype(b)::type, std::false_type>::value,
	// "");

	fea::function<void(int)> f;
	static_assert(std::is_same<decltype(f), std::function<void(int)>>::value,
			"function.cpp : test failed");

	fea::function<void(int), true> f_ptr;
	static_assert(
			std::is_same<decltype(f_ptr), fea::function_cl<void(int)>>::value,
			"function.cpp : test failed");
}
} // namespace


// // helper type for the visitor #4
// template<class... Ts>
// struct overloaded : Ts... { using Ts::operator()...; };
// // explicit deduction guide (not needed as of C++20)
// template<class... Ts>
// overloaded(Ts...) -> overloaded<Ts...>;


// template<class RetT, class... Ts>
// struct overloaded : Ts... {
// 	using Ts::operator()...;

// 	template <class T>
// 	operator T() {
// 		return this->template operator()<T>(std::move(std::string));
// 	}

// 	RetT ret;
// };
// // explicit deduction guide (not needed as of C++20)
// template<class RetT, class... Ts>
// overloaded(RetT, Ts...) -> overloaded<RetT, Ts...>;


// /*
// Example : simdjson 

// std::pair<int, error_code> = bla["bla"]; // noexcept
// int = bla["bla"]; // throwing

// */

// auto func() {
// 	return overloaded{
// 		std::move(std::string),
// 		[]<class T>(std::string&&) noexcept -> std::pair<int, error_code> {},
// 		[]<class T>(std::string&&) -> int {},
// 	};
// }
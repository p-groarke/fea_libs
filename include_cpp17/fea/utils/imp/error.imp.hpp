namespace fea {
std::error_code last_os_error() {
#if FEA_WINDOWS
	return std::error_code{ int(GetLastError()), std::system_category() };
#else
	return std::error_code{ errno, std::system_category() };
#endif
}

std::error_code last_errno_error() {
	return std::error_code{ errno, std::system_category() };
}

void print_error_message(
		const char* func_name, size_t line, const std::error_code& ec) {
	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	fea::print_error_message(func_name, line, msg);
}

void maybe_throw(
		const char* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);

#if !FEA_NOTHROW
	std::string msg
			= "Error Code " + std::to_string(ec.value()) + ". " + ec.message();
	throw std::system_error{ ec, std::string{ func_name } + "("
										 + std::to_string(line) + ")" + " : "
										 + msg };
#else
	std::exit(EXIT_FAILURE);
#endif
}

void maybe_throw_on_os_error(const char* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_os_error());
}

void maybe_throw_on_errno(const char* func_name, size_t line) {
	maybe_throw(func_name, line, fea::last_errno_error());
}

void error_exit(const char* func_name, size_t line, const std::error_code& ec) {
	if (!ec) {
		return;
	}

	fea::print_error_message(func_name, line, ec);
	assert(false);
	std::exit(EXIT_FAILURE);
}

void error_exit_on_os_error(const char* func_name, size_t line) {
	error_exit(func_name, line, fea::last_os_error());
}

void error_exit_on_errno(const char* func_name, size_t line) {
	error_exit(func_name, line, fea::last_errno_error());
}

} // namespace fea

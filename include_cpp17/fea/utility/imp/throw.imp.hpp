namespace fea {
void print_error_message(
		const char* func_name, size_t line, const std::string& message) {
	fprintf(stderr, "%s(%zu) : %s\n", func_name, line, message.c_str());
}

void error_exit(
		const char* func_name, size_t line, const std::string& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);
	std::exit(EXIT_FAILURE);
}

template <class Ex>
void maybe_throw(
		const char* func_name, size_t line, const std::string& message) {
	fea::print_error_message(func_name, line, message);
	assert(false);

#if !FEA_NOTHROW
	throw Ex{ std::string{ func_name } + "(" + std::to_string(line) + ")"
			  + " : " + message };
#else
	std::exit(EXIT_FAILURE);
#endif
}

} // namespace fea

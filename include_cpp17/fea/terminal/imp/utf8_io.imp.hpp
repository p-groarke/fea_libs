#pragma once
namespace fea {
#if !FEA_WINDOWS
codepage_resetter utf8_io(bool) {
	return {};
}
codepage_resetter utf8_io() {
	return {};
}
#else

codepage_resetter utf8_io(bool force_wide) {
	unsigned in_cp_prev = GetConsoleCP();
	if (in_cp_prev == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	unsigned out_cp_prev = GetConsoleOutputCP();
	if (out_cp_prev == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	codepage_resetter ret{ in_cp_prev, out_cp_prev };

	if (SetConsoleCP(CP_UTF8) == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}
	if (SetConsoleOutputCP(CP_UTF8) == 0) {
		fea::maybe_throw_on_os_error(__FUNCTION__, __LINE__);
	}

	if (force_wide) {
		ret.reset_translation(translate_io(translation_mode::u16text));
	}
	return ret;
}

codepage_resetter utf8_io() {
	return utf8_io(false);
}
codepage_resetter::codepage_resetter(unsigned in_cp, unsigned out_cp) noexcept
		: _in_cp(in_cp)
		, _out_cp(out_cp) {
}

codepage_resetter::~codepage_resetter() {
	if (_in_cp != sentinel()) {
		if (SetConsoleCP(_in_cp) == 0) {
			fea::error_exit_on_os_error(__FUNCTION__, __LINE__);
		}
	}
	if (_out_cp != sentinel()) {
		if (SetConsoleOutputCP(_out_cp) == 0) {
			fea::error_exit_on_os_error(__FUNCTION__, __LINE__);
		}
	}
}

codepage_resetter::codepage_resetter(codepage_resetter&& other) noexcept
		: _in_cp(other._in_cp)
		, _out_cp(other._out_cp) {
	other._in_cp = sentinel();
	other._out_cp = sentinel();
}

codepage_resetter& codepage_resetter::operator=(
		codepage_resetter&& other) noexcept {
	if (this != &other) {
		_in_cp = other._in_cp;
		_out_cp = other._out_cp;
		other._in_cp = sentinel();
		other._out_cp = sentinel();
	}
	return *this;
}

void codepage_resetter::reset_translation(translation_resetter&& r) {
	_trans_reset = std::move(r);
}

constexpr unsigned codepage_resetter::sentinel() noexcept {
	return (std::numeric_limits<unsigned>::max)();
}
#endif
} // namespace fea
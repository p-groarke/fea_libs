#pragma once
namespace fea {
#if !FEA_WINDOWS
translation_resetter translate_io(
		translation_mode, translation_mode, translation_mode) {
	return {};
}

translation_resetter translate_io(translation_mode) {
	return {};
}

translation_mode translation_resetter::previous_stdin_mode() const {
	return translation_mode::ignore;
}

translation_mode translation_resetter::previous_stdout_mode() const {
	return translation_mode::ignore;
}

translation_mode translation_resetter::previous_stderr_mode() const {
	return translation_mode::ignore;
}
#else

translation_resetter translate_io(translation_mode in_mode,
		translation_mode out_mode, translation_mode err_mode) {
	int in_prev = -1;
	int out_prev = -1;
	int err_prev = -1;

	if (in_mode != translation_mode::ignore) {
		in_prev = _setmode(_fileno(stdin), int(in_mode));
		if (in_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (out_mode != translation_mode::ignore) {
		if (fflush(stdout) == EOF) {
			fea::maybe_throw(__FUNCTION__, __LINE__, "Couldn't flush stdout.");
		}
		out_prev = _setmode(_fileno(stdout), int(out_mode));
		if (out_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (err_mode != translation_mode::ignore) {
		if (fflush(stderr) == EOF) {
			fea::maybe_throw(__FUNCTION__, __LINE__, "Couldn't flush stderr.");
		}
		err_prev = _setmode(_fileno(stderr), int(err_mode));
		if (err_prev == -1) {
			fea::maybe_throw_on_errno(__FUNCTION__, __LINE__);
		}
	}

	return translation_resetter{
		translation_mode(in_prev),
		translation_mode(out_prev),
		translation_mode(err_prev),
	};
}

translation_resetter translate_io(translation_mode all_mode) {
	return translate_io(all_mode, all_mode, all_mode);
}


translation_resetter::translation_resetter(translation_mode in_mode,
		translation_mode out_mode, translation_mode err_mode) noexcept
		: _in_mode(in_mode)
		, _out_mode(out_mode)
		, _err_mode(err_mode) {
}

translation_resetter::~translation_resetter() {
	if (_in_mode != translation_mode::ignore) {
		if (_setmode(_fileno(stdin), int(_in_mode)) == -1) {
			fea::error_exit_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (_out_mode != translation_mode::ignore) {
		if (fflush(stdout) == EOF) {
			fea::error_exit(__FUNCTION__, __LINE__, "Couldn't flush stdout.");
		}
		if (_setmode(_fileno(stdout), int(_out_mode)) == -1) {
			fea::error_exit_on_errno(__FUNCTION__, __LINE__);
		}
	}

	if (_err_mode != translation_mode::ignore) {
		if (fflush(stderr) == EOF) {
			fea::error_exit(__FUNCTION__, __LINE__, "Couldn't flush stderr.");
		}
		if (_setmode(_fileno(stderr), int(_err_mode)) == -1) {
			fea::error_exit_on_errno(__FUNCTION__, __LINE__);
		}
	}
}

translation_resetter::translation_resetter(
		translation_resetter&& other) noexcept
		: _in_mode(other._in_mode)
		, _out_mode(other._out_mode)
		, _err_mode(other._err_mode) {
	other._in_mode = translation_mode::ignore;
	other._out_mode = translation_mode::ignore;
	other._err_mode = translation_mode::ignore;
}

translation_resetter& translation_resetter::operator=(
		translation_resetter&& other) noexcept {
	if (this != &other) {
		_in_mode = other._in_mode;
		_out_mode = other._out_mode;
		_err_mode = other._err_mode;
		other._in_mode = translation_mode::ignore;
		other._out_mode = translation_mode::ignore;
		other._err_mode = translation_mode::ignore;
	}
	return *this;
}

translation_mode translation_resetter::previous_stdin_mode() const {
	return _in_mode;
}

translation_mode translation_resetter::previous_stdout_mode() const {
	return _out_mode;
}

translation_mode translation_resetter::previous_stderr_mode() const {
	return _err_mode;
}

#endif
} // namespace fea
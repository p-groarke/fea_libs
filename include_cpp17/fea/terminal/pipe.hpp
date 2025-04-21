/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
#include "fea/terminal/translate_io.hpp"
#include "fea/utility/error.hpp"
#include "fea/utility/platform.hpp"
#include "fea/utility/scope.hpp"
#include "fea/utility/unused.hpp"

#include <codecvt>
#include <iostream>
#include <locale>
#include <string>

#if FEA_WINDOWS
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#else
#include <sys/ioctl.h>
#endif

/*
Cross-platform stdin pipe helpers.
*/

namespace fea {
// Disables syncing with C io functions. Makes std::cout and friends fast.
inline void fast_iostreams();

// A non-blocking function that returns the number of bytes available in stdin.
inline size_t available_pipe_bytes();

// If there is any text in application pipe, read it.
inline std::wstring wread_pipe_text();

// If there is any text in application pipe, read it.
inline std::string read_pipe_text();

} // namespace fea

#include "imp/pipe.imp.hpp"

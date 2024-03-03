/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <cmath>

namespace fea {

// Returns the percentage of v, given the range [a, b].
template <class T>
T percentage(const T& v, const T& a, const T& b) {
	return (v - a) / (b - a);
}

// Linearly interpolates between [a, b], given per percentage.
template <class T>
T lerp(const T& per, const T& a, const T& b) {
	return a + per * (b - a);
}

// Linearly interpolates between [to_a, to_b], given v a value between [from_a,
// from_b].
template <class T>
T lerp(const T& v, const T& from_a, const T& from_b, const T& to_a,
		const T& to_b) {
	T per = fea::percentage(v, from_a, from_b);
	return fea::lerp(per, to_a, to_b);
}

// Interpolates in either linear, quadratic or logarithmic fashion.
// k == 0, linear
// k > 0, quadratic
// k < 0, logarithmic
// a is the zero value in y, given x == 0
// b is the upper value in y, given x == 1
// https://www.desmos.com/calculator/ezudd38ncy

template <class T>
T lerp_linquadlog(const T& per, const T& k, T a = T(0), T b = T(1)) {
	if (k == T(0)) {
		return fea::lerp(per, a, b);
	}
	return a + ((std::exp(k * per) - T(1)) / (std::exp(k) - T(1))) * (b - a);
}


} // namespace fea
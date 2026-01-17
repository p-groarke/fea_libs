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

namespace fea {
// Useful English datasets.

namespace en {
namespace detail {
// English letters sorted by frequency according to wikipedia.
inline constexpr std::array<char, 26> letters{ 'e', 't', 'a', 'o', 'i', 'n',
	's', 'h', 'r', 'd', 'l', 'c', 'u', 'm', 'w', 'f', 'g', 'y', 'p', 'b', 'v',
	'k', 'j', 'x', 'q', 'z' };


/*
Approximate, from wikipedia source :
https://web.archive.org/web/20210304152631/http://en.algoritmy.net/article/40379/Letter-frequency-English
E	12.702%
T	9.056%
A	8.167%
O	7.507%
I	6.966%
N	6.749%
S	6.327%
H	6.094%
R	5.987%
D	4.253%
L	4.025%
C	2.782%
U	2.758%
M	2.406%
W	2.360%
F	2.228%
G	2.015%
Y	1.974%
P	1.929%
B	1.492%
V	0.978%
K	0.772%
J	0.153%
X	0.150%
Q	0.095%
Z	0.074%
*/
consteval std::array<double, 256> make_letter_frequencies() {
	std::array<double, 256> ret{};
	ret[size_t('E')] = 0.12702 + 0.00001;
	ret[size_t('e')] = 0.12702 + 0.00001;
	ret[size_t('T')] = 0.09056 + 0.00000000000000011102230246251565;
	ret[size_t('t')] = 0.09056 + 0.00000000000000011102230246251565;
	ret[size_t('A')] = 0.08167;
	ret[size_t('a')] = 0.08167;
	ret[size_t('O')] = 0.07507;
	ret[size_t('o')] = 0.07507;
	ret[size_t('I')] = 0.06966;
	ret[size_t('i')] = 0.06966;
	ret[size_t('N')] = 0.06749;
	ret[size_t('n')] = 0.06749;
	ret[size_t('S')] = 0.06327;
	ret[size_t('s')] = 0.06327;
	ret[size_t('H')] = 0.06094;
	ret[size_t('h')] = 0.06094;
	ret[size_t('R')] = 0.05987;
	ret[size_t('r')] = 0.05987;
	ret[size_t('D')] = 0.04253;
	ret[size_t('d')] = 0.04253;
	ret[size_t('L')] = 0.04025;
	ret[size_t('l')] = 0.04025;
	ret[size_t('C')] = 0.02782;
	ret[size_t('c')] = 0.02782;
	ret[size_t('U')] = 0.02758;
	ret[size_t('u')] = 0.02758;
	ret[size_t('M')] = 0.02406;
	ret[size_t('m')] = 0.02406;
	ret[size_t('W')] = 0.02360;
	ret[size_t('w')] = 0.02360;
	ret[size_t('F')] = 0.02228;
	ret[size_t('f')] = 0.02228;
	ret[size_t('G')] = 0.02015;
	ret[size_t('g')] = 0.02015;
	ret[size_t('Y')] = 0.01974;
	ret[size_t('y')] = 0.01974;
	ret[size_t('P')] = 0.01929;
	ret[size_t('p')] = 0.01929;
	ret[size_t('B')] = 0.01492;
	ret[size_t('b')] = 0.01492;
	ret[size_t('V')] = 0.00978;
	ret[size_t('v')] = 0.00978;
	ret[size_t('K')] = 0.00772;
	ret[size_t('k')] = 0.00772;
	ret[size_t('J')] = 0.00153;
	ret[size_t('j')] = 0.00153;
	ret[size_t('X')] = 0.00150;
	ret[size_t('x')] = 0.00150;
	ret[size_t('Q')] = 0.00095;
	ret[size_t('q')] = 0.00095;
	ret[size_t('Z')] = 0.00074;
	ret[size_t('z')] = 0.00074;
	return ret;
}
// Letter frequencies, indexable by ascii letter.
inline constexpr std::array<double, 256> letter_frequencies
		= make_letter_frequencies();


// Bigrams, sorted by frequency according to google book analysis (Norvig).
inline constexpr std::array<std::string_view, 676> bigrams{ "th", "he", "in",
	"er", "an", "re", "on", "at", "en", "nd", "ti", "es", "or", "te", "of",
	"ed", "is", "it", "al", "ar", "st", "to", "nt", "ng", "se", "ha", "as",
	"ou", "io", "le", "ve", "co", "me", "de", "hi", "ri", "ro", "ic", "ne",
	"ea", "ra", "ce", "li", "ch", "ll", "be", "ma", "si", "om", "ur", "ca",
	"ta", "el", "la", "ns", "di", "fo", "ho", "pe", "ec", "pr", "no", "ct",
	"us", "ac", "ot", "il", "tr", "ly", "nc", "et", "ut", "ss", "so", "rs",
	"un", "lo", "wa", "ie", "ge", "wh", "ee", "wi", "em", "ad", "ol", "rt",
	"we", "po", "na", "ul", "ni", "ts", "mo", "ow", "pa", "mi", "im", "ai",
	"sh", "ir", "su", "id", "os", "iv", "ia", "fi", "am", "ci", "vi", "pl",
	"tu", "ig", "ev", "ld", "ry", "mp", "fe", "bl", "ab", "gh", "ty", "op",
	"wo", "sa", "ay", "ke", "ex", "fr", "oo", "av", "ag", "if", "ap", "gr",
	"od", "bo", "sp", "rd", "uc", "do", "bu", "ei", "ov", "by", "rm", "ep",
	"tt", "oc", "fa", "ef", "cu", "rn", "sc", "gi", "da", "yo", "cr", "cl",
	"qu", "ga", "du", "ue", "ff", "ba", "ey", "ls", "va", "um", "pp", "up",
	"ua", "lu", "go", "ht", "ug", "ru", "ds", "lt", "pi", "rr", "rc", "eg",
	"au", "ck", "ew", "mu", "br", "bi", "pt", "pu", "ak", "ui", "rg", "ib",
	"tl", "ny", "ki", "ys", "rk", "ob", "mm", "fu", "ph", "og", "ye", "ms",
	"ud", "mb", "ub", "ip", "oi", "rl", "gu", "dr", "hr", "cc", "tw", "ft",
	"wn", "nu", "hu", "af", "nn", "eo", "vo", "rv", "xp", "nf", "gn", "sm",
	"fl", "ok", "nl", "iz", "my", "gl", "aw", "ju", "sy", "oa", "eq", "sl",
	"ps", "jo", "lf", "nv", "nk", "je", "kn", "gs", "ze", "hy", "dy", "ks",
	"xt", "bs", "ik", "dd", "rp", "cy", "xi", "sk", "oe", "oy", "ws", "lv",
	"rf", "dl", "wr", "eu", "dg", "xa", "yi", "nm", "rb", "eb", "xc", "tm",
	"tc", "ja", "hn", "gy", "eh", "za", "yp", "gg", "ym", "sw", "lm", "ii",
	"cs", "bj", "xe", "ix", "oh", "lk", "uf", "ox", "lp", "dv", "ax", "dm",
	"yt", "sf", "ka", "iu", "bt", "ya", "pm", "ek", "yl", "wl", "rh", "hs",
	"gt", "yc", "ah", "yn", "rw", "lw", "hm", "hl", "zi", "py", "lc", "az",
	"aj", "ae", "uo", "nj", "nh", "kl", "iq", "bb", "tn", "lr", "gm", "sn",
	"nr", "mn", "fy", "yr", "sb", "dw", "dn", "zo", "yd", "wt", "sq", "oj",
	"lb", "tf", "sr", "nw", "nq", "np", "ln", "lg", "ky", "ko", "fs", "xu",
	"vy", "uy", "uk", "sd", "ml", "hw", "ez", "ej", "dj", "dh", "cq", "ao",
	"yb", "xh", "wd", "ux", "tz", "tp", "nz", "nb", "mf", "mc", "hb", "bv",
	"zz", "yw", "yg", "xy", "xx", "xo", "uv", "tb", "oz", "nx", "mr", "ku",
	"kr", "kh", "kg", "ji", "hd", "gd", "dt", "df", "dc", "db", "cm", "bm",
	"aa", "zy", "zu", "yz", "xv", "xf", "wy", "wf", "vu", "uz", "tg", "sg",
	"lh", "kw", "km", "kf", "ih", "hf", "dp", "cd", "bn", "bd", "bc", "aq",
	"zl", "zh", "yu", "yh", "yf", "xl", "wu", "wp", "wm", "wk", "wc", "wb",
	"vs", "vr", "uu", "uj", "uh", "tv", "td", "sv", "rz", "rx", "rq", "rj",
	"pw", "pn", "pk", "pf", "pd", "pc", "pb", "oq", "mw", "mt", "mh", "mg",
	"md", "kt", "kp", "kd", "kb", "iw", "ij", "hp", "hh", "hc", "gw", "gf",
	"fm", "fg", "fc", "dq", "cz", "cp", "cn", "cg", "cf", "cb", "bp", "bh",
	"zx", "zw", "zv", "zt", "zs", "zr", "zq", "zp", "zn", "zm", "zk", "zj",
	"zg", "zf", "zd", "zc", "zb", "yy", "yx", "yv", "yq", "yk", "yj", "xz",
	"xw", "xs", "xr", "xq", "xn", "xm", "xk", "xj", "xg", "xd", "xb", "wz",
	"wx", "ww", "wv", "wq", "wj", "wg", "vz", "vx", "vw", "vv", "vt", "vq",
	"vp", "vn", "vm", "vl", "vk", "vj", "vh", "vg", "vf", "vd", "vc", "vb",
	"uw", "uq", "tx", "tq", "tk", "tj", "sz", "sx", "sj", "qz", "qy", "qx",
	"qw", "qv", "qt", "qs", "qr", "qq", "qp", "qo", "qn", "qm", "ql", "qk",
	"qj", "qi", "qh", "qg", "qf", "qe", "qd", "qc", "qb", "qa", "pz", "px",
	"pv", "pq", "pj", "pg", "mz", "mx", "mv", "mq", "mk", "mj", "lz", "lx",
	"lq", "lj", "kz", "kx", "kv", "kq", "kk", "kj", "kc", "jz", "jy", "jx",
	"jw", "jv", "jt", "js", "jr", "jq", "jp", "jn", "jm", "jl", "jk", "jj",
	"jh", "jg", "jf", "jd", "jc", "jb", "iy", "hz", "hx", "hv", "hq", "hk",
	"hj", "hg", "gz", "gx", "gv", "gq", "gp", "gk", "gj", "gc", "gb", "fz",
	"fx", "fw", "fv", "fq", "fp", "fn", "fk", "fj", "fh", "fd", "fb", "dz",
	"dx", "dk", "cx", "cw", "cv", "cj", "bz", "bx", "bw", "bq", "bk", "bg",
	"bf" };

// Creates an index based on bigram value.
constexpr size_t bigram_idx(std::string_view bigram) {
	return size_t(bigram[0] - 'a') * 26u + size_t(bigram[1] - 'a');
}

// Returns an array of bigram frequencies, indexed at bigram.
consteval std::array<double, 676> make_bigram_frequencies() {
	// dataset rounding error 0.00005
	std::array<double, 676> ret{};
	ret[bigram_idx("th")] = 0.03556 + 0.0000100000000016653345369377348;
	ret[bigram_idx("he")] = 0.03075 + 0.00001;
	ret[bigram_idx("in")] = 0.02433 + 0.00001;
	ret[bigram_idx("er")] = 0.02048 + 0.00001;
	ret[bigram_idx("an")] = 0.01985 + 0.00001;
	ret[bigram_idx("re")] = 0.01854;
	ret[bigram_idx("on")] = 0.01758;
	ret[bigram_idx("at")] = 0.01487;
	ret[bigram_idx("en")] = 0.01454;
	ret[bigram_idx("nd")] = 0.01352;
	ret[bigram_idx("ti")] = 0.01343;
	ret[bigram_idx("es")] = 0.01339;
	ret[bigram_idx("or")] = 0.01277;
	ret[bigram_idx("te")] = 0.01205;
	ret[bigram_idx("of")] = 0.01175;
	ret[bigram_idx("ed")] = 0.01168;
	ret[bigram_idx("is")] = 0.01128;
	ret[bigram_idx("it")] = 0.01123;
	ret[bigram_idx("al")] = 0.01087;
	ret[bigram_idx("ar")] = 0.01075;
	ret[bigram_idx("st")] = 0.01053;
	ret[bigram_idx("to")] = 0.01041;
	ret[bigram_idx("nt")] = 0.01041;
	ret[bigram_idx("ng")] = 0.00953;
	ret[bigram_idx("se")] = 0.00932;
	ret[bigram_idx("ha")] = 0.00926;
	ret[bigram_idx("as")] = 0.00871;
	ret[bigram_idx("ou")] = 0.00870;
	ret[bigram_idx("io")] = 0.00835;
	ret[bigram_idx("le")] = 0.00829;
	ret[bigram_idx("ve")] = 0.00825;
	ret[bigram_idx("co")] = 0.00794;
	ret[bigram_idx("me")] = 0.00793;
	ret[bigram_idx("de")] = 0.00765;
	ret[bigram_idx("hi")] = 0.00763;
	ret[bigram_idx("ri")] = 0.00728;
	ret[bigram_idx("ro")] = 0.00727;
	ret[bigram_idx("ic")] = 0.00699;
	ret[bigram_idx("ne")] = 0.00692;
	ret[bigram_idx("ea")] = 0.00688;
	ret[bigram_idx("ra")] = 0.00686;
	ret[bigram_idx("ce")] = 0.00651;
	ret[bigram_idx("li")] = 0.00624;
	ret[bigram_idx("ch")] = 0.00598;
	ret[bigram_idx("ll")] = 0.00577;
	ret[bigram_idx("be")] = 0.00576;
	ret[bigram_idx("ma")] = 0.00565;
	ret[bigram_idx("si")] = 0.00550;
	ret[bigram_idx("om")] = 0.00546;
	ret[bigram_idx("ur")] = 0.00543;
	ret[bigram_idx("ca")] = 0.00538;
	ret[bigram_idx("ta")] = 0.00530;
	ret[bigram_idx("el")] = 0.00530;
	ret[bigram_idx("la")] = 0.00528;
	ret[bigram_idx("ns")] = 0.00509;
	ret[bigram_idx("di")] = 0.00493;
	ret[bigram_idx("fo")] = 0.00488;
	ret[bigram_idx("ho")] = 0.00485;
	ret[bigram_idx("pe")] = 0.00478;
	ret[bigram_idx("ec")] = 0.00477;
	ret[bigram_idx("pr")] = 0.00474;
	ret[bigram_idx("no")] = 0.00465;
	ret[bigram_idx("ct")] = 0.00461;
	ret[bigram_idx("us")] = 0.00454;
	ret[bigram_idx("ac")] = 0.00448;
	ret[bigram_idx("ot")] = 0.00442;
	ret[bigram_idx("il")] = 0.00432;
	ret[bigram_idx("tr")] = 0.00426;
	ret[bigram_idx("ly")] = 0.00425;
	ret[bigram_idx("nc")] = 0.00416;
	ret[bigram_idx("et")] = 0.00413;
	ret[bigram_idx("ut")] = 0.00405;
	ret[bigram_idx("ss")] = 0.00405;
	ret[bigram_idx("so")] = 0.00398;
	ret[bigram_idx("rs")] = 0.00397;
	ret[bigram_idx("un")] = 0.00394;
	ret[bigram_idx("lo")] = 0.00387;
	ret[bigram_idx("wa")] = 0.00385;
	ret[bigram_idx("ie")] = 0.00385;
	ret[bigram_idx("ge")] = 0.00385;
	ret[bigram_idx("wh")] = 0.00379;
	ret[bigram_idx("ee")] = 0.00378;
	ret[bigram_idx("wi")] = 0.00374;
	ret[bigram_idx("em")] = 0.00374;
	ret[bigram_idx("ad")] = 0.00368;
	ret[bigram_idx("ol")] = 0.00365;
	ret[bigram_idx("rt")] = 0.00362;
	ret[bigram_idx("we")] = 0.00361;
	ret[bigram_idx("po")] = 0.00361;
	ret[bigram_idx("na")] = 0.00347;
	ret[bigram_idx("ul")] = 0.00346;
	ret[bigram_idx("ni")] = 0.00339;
	ret[bigram_idx("ts")] = 0.00337;
	ret[bigram_idx("mo")] = 0.00337;
	ret[bigram_idx("ow")] = 0.00330;
	ret[bigram_idx("pa")] = 0.00324;
	ret[bigram_idx("mi")] = 0.00318;
	ret[bigram_idx("im")] = 0.00318;
	ret[bigram_idx("ai")] = 0.00316;
	ret[bigram_idx("sh")] = 0.00315;
	ret[bigram_idx("ir")] = 0.00315;
	ret[bigram_idx("su")] = 0.00311;
	ret[bigram_idx("id")] = 0.00296;
	ret[bigram_idx("os")] = 0.00290;
	ret[bigram_idx("iv")] = 0.00288;
	ret[bigram_idx("ia")] = 0.00286;
	ret[bigram_idx("fi")] = 0.00285;
	ret[bigram_idx("am")] = 0.00285;
	ret[bigram_idx("ci")] = 0.00281;
	ret[bigram_idx("vi")] = 0.00270;
	ret[bigram_idx("pl")] = 0.00263;
	ret[bigram_idx("tu")] = 0.00255;
	ret[bigram_idx("ig")] = 0.00255;
	ret[bigram_idx("ev")] = 0.00255;
	ret[bigram_idx("ld")] = 0.00253;
	ret[bigram_idx("ry")] = 0.00248;
	ret[bigram_idx("mp")] = 0.00239;
	ret[bigram_idx("fe")] = 0.00237;
	ret[bigram_idx("bl")] = 0.00233;
	ret[bigram_idx("ab")] = 0.00230;
	ret[bigram_idx("gh")] = 0.00228;
	ret[bigram_idx("ty")] = 0.00227;
	ret[bigram_idx("op")] = 0.00224;
	ret[bigram_idx("wo")] = 0.00222;
	ret[bigram_idx("sa")] = 0.00218;
	ret[bigram_idx("ay")] = 0.00217;
	ret[bigram_idx("ke")] = 0.00214;
	ret[bigram_idx("ex")] = 0.00214;
	ret[bigram_idx("fr")] = 0.00213;
	ret[bigram_idx("oo")] = 0.00210;
	ret[bigram_idx("av")] = 0.00205;
	ret[bigram_idx("ag")] = 0.00205;
	ret[bigram_idx("if")] = 0.00203;
	ret[bigram_idx("ap")] = 0.00203;
	ret[bigram_idx("gr")] = 0.00197;
	ret[bigram_idx("od")] = 0.00195;
	ret[bigram_idx("bo")] = 0.00195;
	ret[bigram_idx("sp")] = 0.00191;
	ret[bigram_idx("rd")] = 0.00189;
	ret[bigram_idx("uc")] = 0.00188;
	ret[bigram_idx("do")] = 0.00188;
	ret[bigram_idx("bu")] = 0.00185;
	ret[bigram_idx("ei")] = 0.00183;
	ret[bigram_idx("ov")] = 0.00178;
	ret[bigram_idx("by")] = 0.00176;
	ret[bigram_idx("rm")] = 0.00175;
	ret[bigram_idx("ep")] = 0.00172;
	ret[bigram_idx("tt")] = 0.00171;
	ret[bigram_idx("oc")] = 0.00166;
	ret[bigram_idx("fa")] = 0.00164;
	ret[bigram_idx("ef")] = 0.00163;
	ret[bigram_idx("cu")] = 0.00163;
	ret[bigram_idx("rn")] = 0.00160;
	ret[bigram_idx("sc")] = 0.00155;
	ret[bigram_idx("gi")] = 0.00152;
	ret[bigram_idx("da")] = 0.00151;
	ret[bigram_idx("yo")] = 0.00150;
	ret[bigram_idx("cr")] = 0.00149;
	ret[bigram_idx("cl")] = 0.00149;
	ret[bigram_idx("qu")] = 0.00148;
	ret[bigram_idx("ga")] = 0.00148;
	ret[bigram_idx("du")] = 0.00148;
	ret[bigram_idx("ue")] = 0.00147;
	ret[bigram_idx("ff")] = 0.00146;
	ret[bigram_idx("ba")] = 0.00146;
	ret[bigram_idx("ey")] = 0.00144;
	ret[bigram_idx("ls")] = 0.00142;
	ret[bigram_idx("va")] = 0.00140;
	ret[bigram_idx("um")] = 0.00138;
	ret[bigram_idx("pp")] = 0.00137;
	ret[bigram_idx("up")] = 0.00136;
	ret[bigram_idx("ua")] = 0.00136;
	ret[bigram_idx("lu")] = 0.00135;
	ret[bigram_idx("go")] = 0.00132;
	ret[bigram_idx("ht")] = 0.00130;
	ret[bigram_idx("ug")] = 0.00128;
	ret[bigram_idx("ru")] = 0.00128;
	ret[bigram_idx("ds")] = 0.00126;
	ret[bigram_idx("lt")] = 0.00124;
	ret[bigram_idx("pi")] = 0.00123;
	ret[bigram_idx("rr")] = 0.00121;
	ret[bigram_idx("rc")] = 0.00121;
	ret[bigram_idx("eg")] = 0.00120;
	ret[bigram_idx("au")] = 0.00119;
	ret[bigram_idx("ck")] = 0.00118;
	ret[bigram_idx("ew")] = 0.00117;
	ret[bigram_idx("mu")] = 0.00115;
	ret[bigram_idx("br")] = 0.00112;
	ret[bigram_idx("bi")] = 0.00107;
	ret[bigram_idx("pt")] = 0.00106;
	ret[bigram_idx("pu")] = 0.00105;
	ret[bigram_idx("ak")] = 0.00105;
	ret[bigram_idx("ui")] = 0.00101;
	ret[bigram_idx("rg")] = 0.00100;
	ret[bigram_idx("ib")] = 0.00099;
	ret[bigram_idx("tl")] = 0.00098;
	ret[bigram_idx("ny")] = 0.00098;
	ret[bigram_idx("ki")] = 0.00098;
	ret[bigram_idx("ys")] = 0.00097;
	ret[bigram_idx("rk")] = 0.00097;
	ret[bigram_idx("ob")] = 0.00097;
	ret[bigram_idx("mm")] = 0.00096;
	ret[bigram_idx("fu")] = 0.00096;
	ret[bigram_idx("ph")] = 0.00094;
	ret[bigram_idx("og")] = 0.00094;
	ret[bigram_idx("ye")] = 0.00093;
	ret[bigram_idx("ms")] = 0.00093;
	ret[bigram_idx("ud")] = 0.00091;
	ret[bigram_idx("mb")] = 0.00090;
	ret[bigram_idx("ub")] = 0.00089;
	ret[bigram_idx("ip")] = 0.00089;
	ret[bigram_idx("oi")] = 0.00088;
	ret[bigram_idx("rl")] = 0.00086;
	ret[bigram_idx("gu")] = 0.00086;
	ret[bigram_idx("dr")] = 0.00085;
	ret[bigram_idx("hr")] = 0.00084;
	ret[bigram_idx("cc")] = 0.00083;
	ret[bigram_idx("tw")] = 0.00082;
	ret[bigram_idx("ft")] = 0.00082;
	ret[bigram_idx("wn")] = 0.00079;
	ret[bigram_idx("nu")] = 0.00079;
	ret[bigram_idx("hu")] = 0.00074;
	ret[bigram_idx("af")] = 0.00074;
	ret[bigram_idx("nn")] = 0.00073;
	ret[bigram_idx("eo")] = 0.00073;
	ret[bigram_idx("vo")] = 0.00071;
	ret[bigram_idx("rv")] = 0.00069;
	ret[bigram_idx("xp")] = 0.00067;
	ret[bigram_idx("nf")] = 0.00067;
	ret[bigram_idx("gn")] = 0.00066;
	ret[bigram_idx("sm")] = 0.00065;
	ret[bigram_idx("fl")] = 0.00065;
	ret[bigram_idx("ok")] = 0.00064;
	ret[bigram_idx("nl")] = 0.00064;
	ret[bigram_idx("iz")] = 0.00064;
	ret[bigram_idx("my")] = 0.00062;
	ret[bigram_idx("gl")] = 0.00061;
	ret[bigram_idx("aw")] = 0.00060;
	ret[bigram_idx("ju")] = 0.00059;
	ret[bigram_idx("sy")] = 0.00057;
	ret[bigram_idx("oa")] = 0.00057;
	ret[bigram_idx("eq")] = 0.00057;
	ret[bigram_idx("sl")] = 0.00056;
	ret[bigram_idx("ps")] = 0.00055;
	ret[bigram_idx("jo")] = 0.00054;
	ret[bigram_idx("lf")] = 0.00053;
	ret[bigram_idx("nv")] = 0.00052;
	ret[bigram_idx("nk")] = 0.00052;
	ret[bigram_idx("je")] = 0.00052;
	ret[bigram_idx("kn")] = 0.00051;
	ret[bigram_idx("gs")] = 0.00051;
	ret[bigram_idx("ze")] = 0.00050;
	ret[bigram_idx("hy")] = 0.00050;
	ret[bigram_idx("dy")] = 0.00050;
	ret[bigram_idx("ks")] = 0.00048;
	ret[bigram_idx("xt")] = 0.00047;
	ret[bigram_idx("bs")] = 0.00046;
	ret[bigram_idx("ik")] = 0.00043;
	ret[bigram_idx("dd")] = 0.00043;
	ret[bigram_idx("rp")] = 0.00042;
	ret[bigram_idx("cy")] = 0.00042;
	ret[bigram_idx("xi")] = 0.00039;
	ret[bigram_idx("sk")] = 0.00039;
	ret[bigram_idx("oe")] = 0.00039;
	ret[bigram_idx("oy")] = 0.00036;
	ret[bigram_idx("ws")] = 0.00035;
	ret[bigram_idx("lv")] = 0.00035;
	ret[bigram_idx("rf")] = 0.00032;
	ret[bigram_idx("dl")] = 0.00032;
	ret[bigram_idx("wr")] = 0.00031;
	ret[bigram_idx("eu")] = 0.00031;
	ret[bigram_idx("dg")] = 0.00031;
	ret[bigram_idx("xa")] = 0.00030;
	ret[bigram_idx("yi")] = 0.00029;
	ret[bigram_idx("nm")] = 0.00028;
	ret[bigram_idx("rb")] = 0.00027;
	ret[bigram_idx("eb")] = 0.00027;
	ret[bigram_idx("xc")] = 0.00026;
	ret[bigram_idx("tm")] = 0.00026;
	ret[bigram_idx("tc")] = 0.00026;
	ret[bigram_idx("ja")] = 0.00026;
	ret[bigram_idx("hn")] = 0.00026;
	ret[bigram_idx("gy")] = 0.00026;
	ret[bigram_idx("eh")] = 0.00026;
	ret[bigram_idx("za")] = 0.00025;
	ret[bigram_idx("yp")] = 0.00025;
	ret[bigram_idx("gg")] = 0.00025;
	ret[bigram_idx("ym")] = 0.00024;
	ret[bigram_idx("sw")] = 0.00024;
	ret[bigram_idx("lm")] = 0.00023;
	ret[bigram_idx("ii")] = 0.00023;
	ret[bigram_idx("cs")] = 0.00023;
	ret[bigram_idx("bj")] = 0.00023;
	ret[bigram_idx("xe")] = 0.00022;
	ret[bigram_idx("ix")] = 0.00022;
	ret[bigram_idx("oh")] = 0.00021;
	ret[bigram_idx("lk")] = 0.00020;
	ret[bigram_idx("uf")] = 0.00019;
	ret[bigram_idx("ox")] = 0.00019;
	ret[bigram_idx("lp")] = 0.00019;
	ret[bigram_idx("dv")] = 0.00019;
	ret[bigram_idx("ax")] = 0.00019;
	ret[bigram_idx("dm")] = 0.00018;
	ret[bigram_idx("yt")] = 0.00017;
	ret[bigram_idx("sf")] = 0.00017;
	ret[bigram_idx("ka")] = 0.00017;
	ret[bigram_idx("iu")] = 0.00017;
	ret[bigram_idx("bt")] = 0.00017;
	ret[bigram_idx("ya")] = 0.00016;
	ret[bigram_idx("pm")] = 0.00016;
	ret[bigram_idx("ek")] = 0.00016;
	ret[bigram_idx("yl")] = 0.00015;
	ret[bigram_idx("wl")] = 0.00015;
	ret[bigram_idx("rh")] = 0.00015;
	ret[bigram_idx("hs")] = 0.00015;
	ret[bigram_idx("gt")] = 0.00015;
	ret[bigram_idx("yc")] = 0.00014;
	ret[bigram_idx("ah")] = 0.00014;
	ret[bigram_idx("yn")] = 0.00013;
	ret[bigram_idx("rw")] = 0.00013;
	ret[bigram_idx("lw")] = 0.00013;
	ret[bigram_idx("hm")] = 0.00013;
	ret[bigram_idx("hl")] = 0.00013;
	ret[bigram_idx("zi")] = 0.00012;
	ret[bigram_idx("py")] = 0.00012;
	ret[bigram_idx("lc")] = 0.00012;
	ret[bigram_idx("az")] = 0.00012;
	ret[bigram_idx("aj")] = 0.00012;
	ret[bigram_idx("ae")] = 0.00012;
	ret[bigram_idx("uo")] = 0.00011;
	ret[bigram_idx("nj")] = 0.00011;
	ret[bigram_idx("nh")] = 0.00011;
	ret[bigram_idx("kl")] = 0.00011;
	ret[bigram_idx("iq")] = 0.00011;
	ret[bigram_idx("bb")] = 0.00011;
	ret[bigram_idx("tn")] = 0.00010;
	ret[bigram_idx("lr")] = 0.00010;
	ret[bigram_idx("gm")] = 0.00010;
	ret[bigram_idx("sn")] = 0.00009;
	ret[bigram_idx("nr")] = 0.00009;
	ret[bigram_idx("mn")] = 0.00009;
	ret[bigram_idx("fy")] = 0.00009;
	ret[bigram_idx("yr")] = 0.00008;
	ret[bigram_idx("sb")] = 0.00008;
	ret[bigram_idx("dw")] = 0.00008;
	ret[bigram_idx("dn")] = 0.00008;
	ret[bigram_idx("zo")] = 0.00007;
	ret[bigram_idx("yd")] = 0.00007;
	ret[bigram_idx("wt")] = 0.00007;
	ret[bigram_idx("sq")] = 0.00007;
	ret[bigram_idx("oj")] = 0.00007;
	ret[bigram_idx("lb")] = 0.00007;
	ret[bigram_idx("tf")] = 0.00006;
	ret[bigram_idx("sr")] = 0.00006;
	ret[bigram_idx("nw")] = 0.00006;
	ret[bigram_idx("nq")] = 0.00006;
	ret[bigram_idx("np")] = 0.00006;
	ret[bigram_idx("ln")] = 0.00006;
	ret[bigram_idx("lg")] = 0.00006;
	ret[bigram_idx("ky")] = 0.00006;
	ret[bigram_idx("ko")] = 0.00006;
	ret[bigram_idx("fs")] = 0.00006;
	ret[bigram_idx("xu")] = 0.00005;
	ret[bigram_idx("vy")] = 0.00005;
	ret[bigram_idx("uy")] = 0.00005;
	ret[bigram_idx("uk")] = 0.00005;
	ret[bigram_idx("sd")] = 0.00005;
	ret[bigram_idx("ml")] = 0.00005;
	ret[bigram_idx("hw")] = 0.00005;
	ret[bigram_idx("ez")] = 0.00005;
	ret[bigram_idx("ej")] = 0.00005;
	ret[bigram_idx("dj")] = 0.00005;
	ret[bigram_idx("dh")] = 0.00005;
	ret[bigram_idx("cq")] = 0.00005;
	ret[bigram_idx("ao")] = 0.00005;
	ret[bigram_idx("yb")] = 0.00004;
	ret[bigram_idx("xh")] = 0.00004;
	ret[bigram_idx("wd")] = 0.00004;
	ret[bigram_idx("ux")] = 0.00004;
	ret[bigram_idx("tz")] = 0.00004;
	ret[bigram_idx("tp")] = 0.00004;
	ret[bigram_idx("nz")] = 0.00004;
	ret[bigram_idx("nb")] = 0.00004;
	ret[bigram_idx("mf")] = 0.00004;
	ret[bigram_idx("mc")] = 0.00004;
	ret[bigram_idx("hb")] = 0.00004;
	ret[bigram_idx("bv")] = 0.00004;
	ret[bigram_idx("zz")] = 0.00003;
	ret[bigram_idx("yw")] = 0.00003;
	ret[bigram_idx("yg")] = 0.00003;
	ret[bigram_idx("xy")] = 0.00003;
	ret[bigram_idx("xx")] = 0.00003;
	ret[bigram_idx("xo")] = 0.00003;
	ret[bigram_idx("uv")] = 0.00003;
	ret[bigram_idx("tb")] = 0.00003;
	ret[bigram_idx("oz")] = 0.00003;
	ret[bigram_idx("nx")] = 0.00003;
	ret[bigram_idx("mr")] = 0.00003;
	ret[bigram_idx("ku")] = 0.00003;
	ret[bigram_idx("kr")] = 0.00003;
	ret[bigram_idx("kh")] = 0.00003;
	ret[bigram_idx("kg")] = 0.00003;
	ret[bigram_idx("ji")] = 0.00003;
	ret[bigram_idx("hd")] = 0.00003;
	ret[bigram_idx("gd")] = 0.00003;
	ret[bigram_idx("dt")] = 0.00003;
	ret[bigram_idx("df")] = 0.00003;
	ret[bigram_idx("dc")] = 0.00003;
	ret[bigram_idx("db")] = 0.00003;
	ret[bigram_idx("cm")] = 0.00003;
	ret[bigram_idx("bm")] = 0.00003;
	ret[bigram_idx("aa")] = 0.00003;
	ret[bigram_idx("zy")] = 0.00002;
	ret[bigram_idx("zu")] = 0.00002;
	ret[bigram_idx("yz")] = 0.00002;
	ret[bigram_idx("xv")] = 0.00002;
	ret[bigram_idx("xf")] = 0.00002;
	ret[bigram_idx("wy")] = 0.00002;
	ret[bigram_idx("wf")] = 0.00002;
	ret[bigram_idx("vu")] = 0.00002;
	ret[bigram_idx("uz")] = 0.00002;
	ret[bigram_idx("tg")] = 0.00002;
	ret[bigram_idx("sg")] = 0.00002;
	ret[bigram_idx("lh")] = 0.00002;
	ret[bigram_idx("kw")] = 0.00002;
	ret[bigram_idx("km")] = 0.00002;
	ret[bigram_idx("kf")] = 0.00002;
	ret[bigram_idx("ih")] = 0.00002;
	ret[bigram_idx("hf")] = 0.00002;
	ret[bigram_idx("dp")] = 0.00002;
	ret[bigram_idx("cd")] = 0.00002;
	ret[bigram_idx("bn")] = 0.00002;
	ret[bigram_idx("bd")] = 0.00002;
	ret[bigram_idx("bc")] = 0.00002;
	ret[bigram_idx("aq")] = 0.00002;
	ret[bigram_idx("zl")] = 0.00001;
	ret[bigram_idx("zh")] = 0.00001;
	ret[bigram_idx("yu")] = 0.00001;
	ret[bigram_idx("yh")] = 0.00001;
	ret[bigram_idx("yf")] = 0.00001;
	ret[bigram_idx("xl")] = 0.00001;
	ret[bigram_idx("wu")] = 0.00001;
	ret[bigram_idx("wp")] = 0.00001;
	ret[bigram_idx("wm")] = 0.00001;
	ret[bigram_idx("wk")] = 0.00001;
	ret[bigram_idx("wc")] = 0.00001;
	ret[bigram_idx("wb")] = 0.00001;
	ret[bigram_idx("vs")] = 0.00001;
	ret[bigram_idx("vr")] = 0.00001;
	ret[bigram_idx("uu")] = 0.00001;
	ret[bigram_idx("uj")] = 0.00001;
	ret[bigram_idx("uh")] = 0.00001;
	ret[bigram_idx("tv")] = 0.00001;
	ret[bigram_idx("td")] = 0.00001;
	ret[bigram_idx("sv")] = 0.00001;
	ret[bigram_idx("rz")] = 0.00001;
	ret[bigram_idx("rx")] = 0.00001;
	ret[bigram_idx("rq")] = 0.00001;
	ret[bigram_idx("rj")] = 0.00001;
	ret[bigram_idx("pw")] = 0.00001;
	ret[bigram_idx("pn")] = 0.00001;
	ret[bigram_idx("pk")] = 0.00001;
	ret[bigram_idx("pf")] = 0.00001;
	ret[bigram_idx("pd")] = 0.00001;
	ret[bigram_idx("pc")] = 0.00001;
	ret[bigram_idx("pb")] = 0.00001;
	ret[bigram_idx("oq")] = 0.00001;
	ret[bigram_idx("mw")] = 0.00001;
	ret[bigram_idx("mt")] = 0.00001;
	ret[bigram_idx("mh")] = 0.00001;
	ret[bigram_idx("mg")] = 0.00001;
	ret[bigram_idx("md")] = 0.00001;
	ret[bigram_idx("kt")] = 0.00001;
	ret[bigram_idx("kp")] = 0.00001;
	ret[bigram_idx("kd")] = 0.00001;
	ret[bigram_idx("kb")] = 0.00001;
	ret[bigram_idx("iw")] = 0.00001;
	ret[bigram_idx("ij")] = 0.00001;
	ret[bigram_idx("hp")] = 0.00001;
	ret[bigram_idx("hh")] = 0.00001;
	ret[bigram_idx("hc")] = 0.00001;
	ret[bigram_idx("gw")] = 0.00001;
	ret[bigram_idx("gf")] = 0.00001;
	ret[bigram_idx("fm")] = 0.00001;
	ret[bigram_idx("fg")] = 0.00001;
	ret[bigram_idx("fc")] = 0.00001;
	ret[bigram_idx("dq")] = 0.00001;
	ret[bigram_idx("cz")] = 0.00001;
	ret[bigram_idx("cp")] = 0.00001;
	ret[bigram_idx("cn")] = 0.00001;
	ret[bigram_idx("cg")] = 0.00001;
	ret[bigram_idx("cf")] = 0.00001;
	ret[bigram_idx("cb")] = 0.00001;
	ret[bigram_idx("bp")] = 0.00001;
	ret[bigram_idx("bh")] = 0.00001;
	return ret;
}
// The bigram frequencies, indexed at bigram (see above).
inline constexpr std::array<double, 676> bigram_frequencies
		= make_bigram_frequencies();


// 10'000 most popular english words.
// Peter Norvig's dataset from google corpus analysis, trimmed.
// https://norvig.com/mayzner.html
// Unique, lower-case, sorted by popularity.
// clang-format off
inline constexpr std::array<std::string_view, 10000> english_dic{
"the","of","and","to","in","a","is","that",
"for","it","as","was","with","be","by","on",
"not","he","i","this","are","or","his","from",
"at","which","but","have","an","had","they","you",
"were","their","one","all","we","can","her","has",
"there","been","if","more","when","will","would","who",
"so","no","she","other","its","may","these","what",
"them","than","some","him","time","into","only","do",
"such","my","new","about","out","also","two","any",
"up","first","could","our","then","most","see","me",
"should","after","said","your","very","between","made","many",
"over","like","those","did","now","even","well","where",
"must","people","through","how","same","work","being","because",
"man","life","before","each","much","under","s","years",
"way","great","state","both","use","upon","own","used",
"however","us","part","good","world","make","three","while",
"long","day","without","just","men","general","during","another",
"little","found","here","system","does","de","down","number",
"case","against","might","still","back","right","know","place",
"every","government","too","high","old","different","states","take",
"year","power","since","given","god","never","social","order",
"water","thus","form","within","small","shall","public","large",
"law","come","less","children","war","point","called","american",
"again","often","go","few","among","important","end","get",
"house","second","though","present","example","himself","women","last",
"say","left","fact","off","set","per","yet","hand",
"came","development","thought","always","far","country","york","school",
"information","group","following","think","others","political","human","history",
"united","give","family","find","need","figure","possible","although",
"rather","later","university","once","course","until","several","national",
"whole","chapter","early","four","home","means","things","process",
"nature","above","therefore","having","certain","control","data","name",
"act","change","value","body","study","et","table","become",
"whether","city","book","taken","side","times","away","c",
"known","period","best","b","line","am","court","john",
"days","put","young","seen","common","person","either","let",
"why","land","head","business","company","church","words","effect",
"society","around","better","nothing","took","white","itself","something",
"light","question","almost","went","interest","mind","next","least",
"level","themselves","economic","child","death","service","view","action",
"five","ii","press","father","further","love","research","area",
"true","education","self","age","necessary","subject","want","cases",
"ever","going","problem","free","done","making","party","king",
"together","century","section","using","position","type","result","help",
"individual","already","matter","full","half","various","sense","look",
"based","whose","english","south","total","class","became","perhaps",
"london","policy","members","al","local","enough","particular","rate",
"air","along","mother","knowledge","face","word","kind","open",
"terms","able","money","experience","conditions","support","la","problems",
"real","black","language","results","room","force","held","low",
"according","usually","north","show","night","told","short","field",
"reason","asked","quite","nor","health","special","thing","analysis",
"eyes","especially","lord","woman","major","similar","care","theory",
"d","brought","whom","office","art","production","sometimes","third",
"shown","british","due","international","began","single","natural","got",
"account","cause","community","saw","heart","soon","changes","studies",
"groups","method","evidence","seems","greater","required","trade","foreign",
"west","clear","model","e","near","students","probably","french",
"gave","including","read","england","material","term","past","report",
"considered","future","higher","structure","fig","available","working","felt",
"tell","amount","really","function","keep","indeed","growth","market",
"non","increase","personal","cost","mean","surface","knew","idea",
"lower","note","program","treatment","six","food","t","close",
"systems","blood","population","central","character","president","energy","property",
"living","provide","specific","science","return","practice","hands","role",
"countries","management","toward","son","says","generally","influence","purpose",
"america","received","strong","call","services","rights","current","m",
"believe","effects","letter","x","looked","story","forms","seemed",
"ground","main","paper","works","areas","modern","provided","moment",
"written","situation","turn","feet","plan","parts","values","movement",
"private","late","size","union","described","east","test","difficult",
"feel","river","poor","attention","books","town","space","o",
"price","turned","rule","percent","activity","across","play","building",
"anything","physical","capital","n","hard","makes","sea","cent",
"approach","pressure","finally","military","middle","sir","longer","spirit",
"continued","basis","army","red","alone","simple","below","series",
"source","increased","sent","particularly","earth","months","department","heard",
"questions","likely","lost","complete","behind","taking","wife","lines",
"object","hours","twenty","established","committee","related","range","truth",
"income","instead","beyond","rest","developed","outside","organization","religious",
"board","live","l","design","needs","persons","except","authority",
"patient","respect","latter","sure","culture","relationship","ten","methods",
"followed","william","condition","points","addition","direct","seem","industry",
"college","friends","beginning","hundred","manner","front","original","patients",
"appear","include","shows","ways","activities","relations","writing","council",
"disease","standard","fire","german","france","degree","towards","produced",
"leave","understand","cells","average","march","carried","length","difference",
"simply","normal","vol","quality","street","run","answer","morning",
"loss","doing","stage","pay","today","decision","labor","page",
"v","published","workers","bank","top","wanted","journal","passed",
"door","p","importance","western","tax","involved","solution","hope",
"voice","reading","obtained","bring","peace","needed","placed","chief",
"species","added","europe","looking","factors","laws","behavior","die",
"response","led","association","performance","road","issue","consider","equal",
"learning","india","training","forces","cut","earlier","basic","member",
"friend","lead","appears","types","schools","music","james","temperature",
"christian","volume","kept","review","significant","j","former","meaning",
"associated","wrote","center","direction","everything","job","understanding","region",
"big","hold","opinion","text","june","date","application","author",
"f","cell","distribution","fall","becomes","washington","limited","indian",
"presence","sound","iii","meeting","clearly","expected","federal","r",
"nearly","ed","extent","parents","yes","ideas","medical","observed",
"actually","final","george","miles","elements","list","project","product",
"throughout","christ","discussion","applied","rules","produce","religion","deep",
"operation","issues","else","mass","coming","determined","european","events",
"security","oil","fine","distance","move","effective","paid","nation",
"step","gives","success","literature","products","neither","resources","follow",
"cross","ask","levels","immediately","certainly","principle","moral","july",
"formed","reached","faith","deal","recent","legal","administration","comes",
"materials","potential","presented","congress","civil","directly","meet","met",
"principles","expression","myself","born","wide","post","justice","strength",
"relation","statement","financial","dead","larger","cultural","historical","existence",
"built","weight","reference","feeling","round","reported","risk","appeared",
"included","scale","individuals","supply","million","differences","write","doubt",
"hour","industrial","april","primary","base","letters","interests","numbers",
"seven","places","wall","complex","entire","try","article","county",
"costs","thinking","record","flow","follows","sun","attempt","bad",
"instance","commission","easily","unit","died","green","charles","week",
"demand","acid","environment","proper","takes","positive","talk","active",
"image","plant","freedom","library","dark","key","mentioned","co",
"speak","allowed","whatever","concerned","measure","lives","merely","fear",
"paris","independent","heat","circumstances","thomas","giving","master","tried",
"china","eye","upper","gone","actual","cold","frequently","eight",
"ability","unless","regard","henry","status","hear","thousand","minutes",
"parties","stock","student","rise","arms","factor","charge","easy",
"created","stand","started","content","share","picture","herself","agreement",
"remember","constant","none","popular","appropriate","style","start","goods",
"considerable","ready","y","occur","notes","forward","rates","returned",
"defined","construction","minister","useful","remain","nations","island","stood",
"choice","district","speech","economy","jesus","moved","powers","paul",
"desire","division","bill","germany","highly","task","staff","additional",
"progress","site","reasons","serious","re","gold","boy","brother",
"remained","january","sort","relative","pass","previous","pain","separate",
"planning","lay","exchange","der","processes","robert","critical","animals",
"ancient","functions","october","august","sufficient","discussed","marriage","remains",
"programs","introduction","effort","memory","girl","sources","internal","soul",
"news","bed","le","des","brown","caused","chinese","lack",
"husband","september","lived","learn","command","politics","december","duty",
"teacher","daily","technology","essential","allow","box","iron","failure",
"decided","concept","noted","names","getting","pattern","ago","phase",
"teachers","otherwise","variety","determine","hence","blue","prepared","figures",
"contact","lot","h","continue","measures","heavy","labour","companies",
"suggested","southern","subjects","features","communication","fully","officers","wish",
"opportunity","computer","exercise","police","create","reality","add","worked",
"learned","inside","description","teaching","summer","gas","car","scientific",
"title","soil","claim","objects","village","trying","traditional","protection",
"entirely","employed","soviet","fixed","lady","provides","efforts","capacity",
"completely","expressed","majority","iv","principal","purposes","mental","access",
"saying","raised","reduced","families","changed","compared","practical","successful",
"interesting","code","causes","increasing","plants","impossible","growing","facts",
"develop","color","begin","someone","event","somewhat","month","offered",
"notice","revolution","philosophy","leading","showed","ones","affairs","beautiful",
"relatively","covered","employment","offer","november","operations","wrong","negative",
"examples","require","acts","attack","tree","address","institutions","aid",
"africa","david","proposed","hall","motion","g","receive","till",
"carry","connection","female","serve","weeks","stone","quickly","constitution",
"formation","evening","race","animal","procedure","collection","annual","royal",
"secretary","rich","en","male","professional","greatest","context","trust",
"units","trees","mary","appearance","firm","balance","britain","spring",
"stop","reaction","characteristics","leaders","sample","classes","foot","battle",
"contract","thirty","accepted","japanese","matters","resistance","hair","believed",
"drawn","reports","standing","equipment","california","wood","floor","married",
"argument","file","mark","smith","miss","hill","opened","daughter",
"despite","properties","conference","oh","glass","advantage","external","prevent",
"goes","output","judge","houses","stated","represented","investment","enemy",
"papers","japan","piece","credit","commercial","northern","square","reach",
"actions","contains","meant","hospital","conflict","native","visit","february",
"existing","designed","judgment","fell","sex","entered","film","initial",
"records","official","african","holy","requires","enter","explain","happened",
"introduced","worth","sat","roman","concerning","educational","treated","officer",
"element","prices","fair","skin","aspects","conduct","cover","ordinary",
"ship","forth","double","contrast","responsibility","moving","avoid","technical",
"mouth","k","running","exist","network","impact","responsible","rose",
"arrived","portion","occurs","requirements","intended","machine","search","steps",
"views","leaves","understood","served","san","spent","definition","powerful",
"governor","game","scene","filled","brain","chance","shape","origin",
"spiritual","numerous","happy","obtain","window","cities","closed","fish",
"stress","setting","standards","ibid","professor","sexual","eastern","older",
"smaller","consideration","trial","speaking","models","winter","correct","hot",
"boys","stay","removed","plans","perfect","sales","leaving","accept",
"fields","usual","prior","captain","suddenly","apply","domestic","tradition",
"frequency","marked","horse","speed","benefit","concern","wind","index",
"difficulty","birth","moreover","passage","ought","relationships","laid","uses",
"corporation","organizations","transfer","selected","regular","agreed","named","error",
"fourth","maximum","conclusion","joint","dry","bodies","sign","divine",
"richard","lake","focus","bear","institute","techniques","chicago","regarded",
"referred","survey","centre","peter","recently","examination","chemical","whereas",
"orders","jewish","suppose","ill","nevertheless","girls","hardly","decisions",
"everyone","highest","queen","forced","urban","bound","tests","exactly",
"performed","equation","details","patterns","troops","possibility","includes","absence",
"feelings","indicated","rock","represent","proved","kinds","kingdom","bottom",
"policies","largely","apparently","structures","maintain","greek","coast","interpretation",
"attitude","canada","station","van","equally","opposite","divided","prince",
"evil","plate","adopted","gain","comparison","edition","developing","bit",
"agricultural","spread","explained","path","immediate","message","net","check",
"connected","walls","russian","pure","break","holding","ratio","environmental",
"claims","send","suggest","containing","sides","secondary","reader","occurred",
"sight","stories","americans","anyone","media","belief","benefits","concentration",
"dear","opening","indicate","nine","generation","affected","team","save",
"farm","discovered","forest","doctor","recognized","advanced","assembly","assumed",
"occasion","interested","danger","mere","broad","indians","insurance","broken",
"identity","pleasure","spanish","agent","yourself","courts","aware","played",
"failed","medium","youth","director","extended","empire","derived","clinical",
"supposed","pages","literary","killed","proportion","safety","arm","writer",
"alternative","sleep","items","struggle","und","obvious","fresh","knows",
"fit","showing","anti","articles","drug","sum","drive","finding",
"rome","previously","carefully","selection","assistance","expect","employees","leader",
"parliament","cambridge","park","accounts","trouble","slowly","valley","fast",
"typical","rural","appointed","election","supreme","closely","estate","variable",
"citizens","oxford","characteristic","formal","besides","metal","pointed","foundation",
"guide","mode","steel","express","agency","sold","contained","skills",
"doctrine","wild","les","vision","opposition","slightly","secret","lies",
"silver","democratic","straight","banks","increases","choose","fundamental","sought",
"beauty","directed","location","spoke","sister","heaven","thoughts","reform",
"grand","apart","detail","contain","assume","prove","fight","soldiers",
"edge","career","strategy","supported","consciousness","creation","catholic","severe",
"background","suggests","walk","boston","movements","describe","bar","fifty",
"latin","mountain","establish","composition","garden","w","grant","violence",
"quantity","russia","camp","strange","famous","intellectual","du","provisions",
"studied","regions","extremely","measured","changing","poetry","apparent","rapidly",
"greatly","excellent","components","brief","solid","becoming","reduce","sections",
"false","identified","absolute","ordered","dependent","chosen","evaluation","seeing",
"declared","writers","experiences","jews","funds","mission","drawing","significance",
"mexico","difficulties","leadership","experimental","ourselves","procedures","intelligence","passing",
"practices","advance","technique","operating","universal","authorities","executive","references",
"ran","relief","regarding","possibly","thou","confidence","reduction","appeal",
"draw","regional","minimum","assessment","multiple","familiar","independence","defense",
"seek","combination","capable","contrary","el","combined","pieces","variables",
"organized","duties","explanation","republic","superior","italy","sale","consists",
"un","twelve","forty","contemporary","wealth","necessarily","depends","please",
"medicine","limits","build","ideal","wave","mine","parallel","characters",
"reasonable","subsequent","objective","located","symptoms","treaty","height","experiments",
"lands","tissue","convention","mm","signs","fellow","resolution","version",
"pre","rapid","crime","lie","inner","eventually","stopped","competition",
"psychology","u","waiting","talking","therapy","soft","corresponding","signal",
"limit","replied","calls","taught","goal","grow","payment","ends",
"engaged","completed","joseph","minute","copy","affect","distinct","campaign",
"protein","plane","applications","represents","buildings","sector","slow","agents",
"walked","demands","edward","column","sentence","necessity","images","von",
"novel","substance","consequence","louis","adult","printed","consequences","psychological",
"experiment","vi","approximately","block","store","statements","layer","yellow",
"observations","season","israel","personality","plain","desired","arts","maybe",
"keeping","permanent","welfare","territory","mainly","museum","buy","narrow",
"cast","watch","answered","communities","crisis","providing","tend","authors",
"feature","scheme","recorded","positions","agriculture","count","cash","bishop",
"profit","projects","possession","perspective","distinction","resulting","thy","safe",
"primarily","naturally","touch","aspect","sitting","carrying","settlement","recognition",
"branch","grace","institution","milk","johnson","unable","joined","shot",
"officials","controlled","audience","proof","liberty","extreme","pretty","granted",
"bone","issued","agree","dollars","establishment","transport","salt","crown",
"circle","spain","maintained","extensive","remaining","warm","relevant","mountains",
"damage","hearing","gradually","temple","determination","representation","publication","achieved",
"helped","observation","repeated","attempts","radio","wait","refused","windows",
"leads","looks","summary","vast","evident","emphasis","train","input",
"baby","partly","detailed","minor","ice","travel","thin","motor",
"improvement","unknown","eat","periods","depth","grew","poet","escape",
"client","caught","cm","experienced","user","sequence","ministry","valuable",
"nuclear","port","willing","percentage","strongly","document","band","conversation",
"ships","emperor","exists","tendency","sons","finished","advice","overall",
"wants","secure","cancer","instrument","request","mixed","sit","situations",
"afterwards","perform","academic","estimated","preparation","virginia","consequently","begins",
"cycle","attached","global","widely","instruction","proceedings","provision","manager",
"italian","commonly","purchase","sets","sites","mr","investigation","dog",
"bridge","properly","circuit","component","select","composed","obviously","sin",
"interaction","ages","respectively","curve","density","criticism","map","careful",
"settled","concepts","criminal","electric","knowing","turning","angle","largest",
"goals","heads","weak","load","debt","distinguished","club","protect",
"plus","identify","corner","engineering","facilities","dream","vote","fairly",
"industries","documents","web","adequate","fund","evolution","tone","processing",
"asia","entry","expansion","birds","happen","emotional","entitled","frame",
"duke","efficiency","opportunities","dangerous","achieve","acting","grown","representative",
"inches","governments","equivalent","vessels","sugar","song","collected","similarly",
"discuss","commerce","bay","testing","islands","bright","administrative","horses",
"flat","coal","indicates","acquired","unity","firms","conducted","legislation",
"liquid","muscle","mention","owner","truly","couple","que","playing",
"editor","shared","participation","continuous","loved","taxes","flowers","yield",
"spot","unique","release","stages","starting","interview","stream","irish",
"suitable","ireland","markets","rare","improve","boat","notion","television",
"chair","tools","weather","remove","hotel","liberal","centuries","democracy",
"lose","stands","consumption","maintenance","thick","occupied","michael","contents",
"fluid","tube","display","regulations","opposed","realized","permission","seat",
"prayer","refer","promise","visual","colonial","pictures","fifth","fruit",
"remarkable","dinner","elsewhere","root","won","journey","silence","bearing",
"raise","quiet","st","virtue","probability","martin","suffering","twice",
"extension","star","mechanical","mid","ring","taste","occasionally","committed",
"substantial","allows","housing","theoretical","parent","mechanism","regulation","thousands",
"wisdom","join","ball","worse","drugs","chain","degrees","los",
"improved","instances","calculated","injury","target","sell","responses","founded",
"exception","neck","zone","visible","accompanied","concerns","dealing","rising",
"agencies","arrangement","readily","admitted","plays","clean","towns","argued",
"recognize","drop","theories","pair","quarter","spite","solutions","sharp",
"quoted","poem","churches","concrete","afternoon","essentially","contribution","producing",
"involves","ad","examined","wonder","consistent","variation","challenge","bible",
"waters","revealed","attitudes","shift","nineteenth","streets","informed","structural",
"elected","zero","sciences","exposed","item","laboratory","province","atmosphere",
"di","revenue","session","waste","fort","fat","fashion","approaches",
"illustrated","personnel","ahead","sky","communist","frequent","originally","vice",
"societies","prime","fighting","wise","thereby","device","charged","poverty",
"wine","readers","grounds","representatives","household","transition","behaviour","software",
"pacific","diseases","carbon","amounts","grade","channel","jobs","mail",
"core","creating","offers","sec","frank","oxygen","claimed","tables",
"discovery","perfectly","branches","cotton","op","colonel","aside","joy",
"suffered","sand","sweet","categories","farmers","victory","wages","drink",
"younger","minds","decide","destruction","ensure","whenever","increasingly","beneath",
"impression","constitutional","colour","reactions","corporate","fifteen","begun","rain",
"wilson","texas","extra","arguments","organic","gray","driven","noise",
"instructions","sacred","velocity","afraid","satisfaction","translation","imagine","destroyed",
"guard","strategies","estimate","classical","onto","statistics","enterprise","depression",
"instruments","languages","favor","meetings","calling","surely","unlike","quick",
"sunday","nerve","sub","conscious","brothers","framework","strike","rooms",
"narrative","managed","skill","ocean","tells","huge","ended","depend",
"logic","enjoy","assets","examine","findings","expense","teeth","budget",
"painting","manufacturing","separated","directions","furthermore","advantages","finds","empty",
"route","thank","category","appendix","intensity","chamber","artist","lying",
"measurement","assigned","constantly","drew","struck","assumption","mostly","delivered",
"debate","clothes","acute","recommended","smile","armed","induced","snow",
"moon","host","legs","inch","rocks","qualities","pope","rank",
"imagination","conception","normally","interior","vertical","remembered","concluded","permitted",
"rarely","decline","seeking","uniform","fail","excess","flight","philadelphia",
"engine","permit","via","telephone","gained","demonstrated","dance","jones",
"exposure","sounds","pounds","diagnosis","era","succeeded","deeply","elizabeth",
"defence","bringing","sudden","hit","telling","employee","falls","specifically",
"screen","diameter","gift","australia","separation","traffic","senate","signed",
"presents","broke","bond","objectives","alexander","league","theatre","storage",
"sheet","namely","supplies","devoted","formula","intention","arranged","jack",
"chapters","charges","egypt","specified","happiness","noble","attempted","fill",
"preceding","efficient","satisfied","precisely","vessel","forget","dress","discipline",
"teach","ex","spend","infection","answers","errors","linear","witness",
"distributed","considering","phone","reflected","particles","prison","aim","significantly",
"marketing","functional","card","print","bird","lee","scope","replaced",
"slave","accordingly","mutual","putting","border","defendant","worship","involving",
"link","walking","receiving","economics","closer","stars","pulled","newspaper",
"criteria","stable","inhabitants","mixture","expenses","slight","consumer","alcohol",
"samples","copper","compare","tasks","unto","anxiety","berlin","golden",
"returns","sick","se","perception","beings","resource","define","finance",
"reflect","sufficiently","vii","identification","asking","ray","spoken","grain",
"peculiar","universe","gender","sensitive","honor","suit","thee","arise",
"magazine","noticed","distant","radical","describes","observe","christianity","hills",
"wage","classification","exact","kill","holds","owned","vary","axis",
"continues","row","accurate","tool","revolutionary","reply","gets","addressed",
"adults","bread","waves","planned","alive","constructed","opinions","recovery",
"theme","absolutely","cited","listen","determining","parameters","turns","considerations",
"beside","partial","shop","del","worker","railway","bought","glory",
"canadian","saint","cooperation","architecture","ultimately","consent","vital","altogether",
"roots","coffee","poems","nervous","ear","abstract","conclusions","phenomena",
"reserve","collective","ministers","ml","imperial","q","breath","hypothesis",
"intervention","grass","silent","relating","arrival","attended","thanks","logical",
"refers","punishment","trip","devices","climate","outer","biological","definite",
"stability","wished","equilibrium","decrease","oral","amendment","depending","fuel",
"writings","wonderful","dominant","tom","jurisdiction","lips","ye","ultimate",
"boundary","threat","membrane","tears","meat","communications","isolated","cf",
"implications","realize","dropped","cup","bonds","homes","visited","offices",
"variations","magnetic","slaves","marks","watched","transportation","sees","christians",
"unfortunately","integration","majesty","listed","happens","faced","preferred","faces",
"typically","track","cattle","dutch","favour","enemies","fate","conventional",
"illness","abuse","williams","delivery","striking","gods","accomplished","seriously",
"eds","enable","identical","surprise","profits","clay","phenomenon","temporary",
"acceptance","electron","bureau","wire","managers","statistical","custom","shore",
"shares","radiation","bell","edited","transformation","prepare","option","promised",
"arrangements","likewise","faculty","creative","mg","tea","removal","prominent",
"falling","gross","childhood","unusual","rice","easier","steam","dynamic",
"gun","shoulder","chairman","existed","il","fingers","dramatic","ca",
"effectively","random","scarcely","compensation","violent","belong","hell","programme",
"negro","delay","chronic","hoped","priest","discourse","dimensions","regime",
"equations","gentleman","partner","rational","binding","tension","ethnic","operate",
"gospel","pride","treat","contributions","musical","payments","diet","forming",
"influenced","phrase","texts","grave","admit","enjoyed","win","shaped",
"proceed","entrance","matrix","abroad","perceived","produces","everywhere","cutting",
"si","mile","wear","occupation","measurements","passion","involve","execution",
"cloth","hole","approved","domain","characterized","palace","desirable","publications",
"supplied","resulted","customer","courses","strain","customers","remarks","crowd",
"honour","tongue","darkness","files","sake","controls","extraordinary","differ",
"imposed","handle","publishing","users","glad","voltage","fallen","nursing",
"electronic","ma","slavery","trained","raw","discharge","suffer","senior",
"writes","kitchen","loan","arthur","electrical","confusion","membership","conservative",
"runs","pleased","physician","surrounding","nice","wholly","ann","possessed",
"modified","roles","uncle","millions","outcome","extend","practically","seed",
"ohio","corn","reaching","counter","discover","proposal","watching","paragraph",
"selling","peoples","convinced","comments","colonies","adding","muscles","linked",
"gate","incident","roads","options","estimates","legislative","chest","comprehensive",
"losses","scotland","continuing","implementation","underlying","clause","francisco","smooth",
"commander","advertising","talked","saved","score","encouraged","murder","railroad",
"considerably","sacrifice","reflection","demanded","generated","weapons","mechanisms","scholars",
"thrown","minority","declaration","presentation","accounting","hidden","owners","possess",
"probable","customs","announced","carolina","copyright","eating","navy","brings",
"worthy","cried","masses","transmission","dna","statute","pro","flesh",
"accordance","colony","rendered","gathered","feed","friendly","export","viewed",
"beliefs","blind","anger","philip","translated","enormous","philosophical","healthy",
"constitute","socialist","pennsylvania","scott","essay","driving","tall","rejected",
"peak","liability","castle","assistant","primitive","factory","fortune","republican",
"strategic","horizontal","breast","burning","recall","fault","achievement","commitment",
"z","viii","accident","terrible","academy","courage","internet","machinery",
"canal","judicial","participants","cognitive","denied","testimony","subsequently","conversion",
"pupils","thoroughly","click","mill","promote","profession","varied","burden",
"districts","habits","eternal","kings","moments","developments","formerly","civilization",
"remote","quantities","disorders","rough","habit","plot","machines","limitations",
"schedule","servants","sorry","attend","contribute","involvement","spirits","inquiry",
"survival","fed","reputation","lewis","shock","attacks","associations","symbol",
"duration","attorney","massachusetts","dust","villages","catch","francis","par",
"ownership","infant","forgotten","don","comparative","comment","gene","cabinet",
"fly","unions","pleasant","painted","string","raising","disorder","artists",
"moves","speaker","friendship","possibilities","liver","moderate","comfort","offering",
"plates","resolved","surgery","printing","protected","confirmed","cry","verse",
"contracts","games","awareness","lincoln","prisoners","judges","stones","cultures",
"beach","circulation","worst","chiefly","afford","satisfactory","preserved","jackson",
"correspondence","taylor","earl","curious","entering","cars","encourage","reign",
"films","residence","successfully","samuel","shell","decade","released","songs",
"valid","humanity","cool","appointment","doors","trace","virtually","fiction",
"occasions","complicated","representing","hundreds","respond","passes","steady","sixty",
"nose","abandoned","liked","employer","pick","wishes","roof","surprised",
"guilty","eggs","shortly","organizational","surrounded","contributed","confined","allowing",
"racial","expectations","centers","substances","smiled","volumes","wet","overcome",
"beam","wing","intense","fought","picked","progressive","fever","nurse",
"amongst","lateral","guns","walter","returning","newspapers","earliest","obliged",
"transferred","sphere","boundaries","throw","rent","approval","hearts","stores",
"rivers","molecular","hurt","adjustment","desert","alliance","seldom","bones",
"correlation","miller","accuracy","serving","drama","mothers","breaking","busy",
"columbia","naval","nobody","wars","genius","totally","expenditure","decades",
"currently","corps","servant","storm","expensive","mistake","genetic","marine",
"ears","destroy","influences","discrimination","drinking","elections","soldier","requirement",
"match","hath","beat","varying","leg","arab","woods","hydrogen",
"dreams","crop","atlantic","compounds","columns","scientists","ladies","cook",
"mexican","transactions","purely","owing","thinks","interval","anxious","scores",
"emergency","behalf","lieutenant","organs","restricted","smoke","fleet","hopes",
"med","plaintiff","est","everybody","equality","departments","cit","hero",
"diagram","manual","headed","wheat","crossed","dogs","jean","argue",
"syndrome","integrated","loose","deposits","traditions","billion","competitive","clients",
"absorption","mankind","artificial","stronger","newly","invited","distinguish","legislature",
"guess","trans","aircraft","dose","cat","honest","whilst","copies",
"theology","networks","versus","organ","secured","belonging","tons","vehicle",
"applicable","serves","hat","shadow","divisions","essence","helpful","gap",
"heavily","directors","register","compound","provinces","praise","evidently","mining",
"sheep","orientation","reducing","cards","conviction","reserved","sentences","suggestion",
"educated","assist","eighteenth","clothing","meanwhile","jury","twentieth","wounded",
"empirical","angry","sodium","topic","feels","lesson","temperatures","mount",
"covering","dying","pushed","usa","supporting","harvard","tested","implies",
"crops","landscape","ease","tribes","trend","molecules","stomach","shut",
"thereof","finger","critics","citizen","utility","germans","expert","acted",
"monthly","tower","demonstrate","davis","pull","welcome","buried","inferior",
"surfaces","fathers","threatened","obligation","exhibit","loans","blow","discussions",
"conscience","guidance","expressions","tends","reasoning","database","acceptable","reveal",
"pipe","ch","anterior","trading","spectrum","attractive","blocks","moscow",
"retained","aged","quarters","institutional","ethical","simultaneously","somewhere","daniel",
"laughed","farther","productivity","excessive","testament","stations","paying","briefly",
"magnitude","dressed","lights","crucial","infinite","helps","establishing","listening",
"illinois","speaks","fishing","jerusalem","sixth","securities","elementary","handling",
"oriented","ms","shoulders","causing","precise","expedition","den","synthesis",
"attacked","pale","arch","da","defeat","lists","initially","dated",
"grey","flying","flower","scenes","chap","facing","dependence","essays",
"emotions","males","helping","consisting","victim","server","adapted","currency",
"dispute","lawyer","somehow","stored","genuine","shakespeare","acres","proud",
"asian","comfortable","con","designs","rear","hung","negotiations","shook",
"purchased","split","christmas","assured","applies","layers","ethics","freely",
"acids","sympathy","thickness","symbols","preliminary","maintaining","assumptions","pour",
"apparatus","assuming","feeding","width","wooden","diversity","repeat","concentrated",
"wound","universities","recording","kids","reverse","counsel","populations","michigan",
"accused","tiny","handed","harry","lowest","connections","mirror","intermediate",
"effectiveness","undoubtedly","dollar","focused","chart","candidate","leaf","exports",
"bull","strictly","coat","succession","scattered","chose","trends","submitted",
"min","plastic","situated","wheel","drove","equity","attributed","generations",
"orange","interactions","exercises","gentlemen","faithful","interpreted","preserve","absent",
"vols","insisted","corporations","mathematical","neutral","pressed","weakness","unemployment",
"colors","warning","id","tail","touched","sports","flows","missing",
"preparing","liable","productive","tired","adam","acquisition","departure","justified",
"lawrence","draft","regularly","farmer","inspired","earnings","respective","dignity",
"singing","arc","roll","convenient","curriculum","proteins","dozen","priests",
"passages","meal","colleagues","saving","occurrence","deeper","routine","profile",
"spending","wider","intervals","aids","displayed","harm","pray","sole",
"prefer","cape","systematic","ion","municipal","sad","partners","render",
"wearing","masters","attributes","ward","pool","waited","curves","converted",
"ll","yards","deny","illustration","tv","posterior","plasma","avenue",
"interface","tied","ph","ah","creek","adams","brilliant","pregnancy",
"voices","enterprises","threw","fun","tale","measuring","instant","manage",
"stephen","conclude","voluntary","colored","solve","females","affection","terminal",
"fee","alike","disappeared","physics","organisation","executed","surprising","approached",
"panel","victims","restrictions","sharing","eleven","obligations","mathematics","harmony",
"proposition","dealt","florida","transaction","links","ride","regardless","sword",
"hunting","allen","classroom","applying","thesis","proceeded","throne","studying",
"participate","gay","candidates","residents","repair","particle","cap","channels",
"migration","provincial","magic","desk","conservation","altered","firmly","excited",
"modes","treasury","affair","pocket","stayed","verbal","capture","profound",
"circular","validity","outline","souls","guy","messages","format","gifts",
"desires","satisfy","ix","inevitable","dr","emerged","bag","coefficient",
"exclusive","doctors","suggestions","mood","agreements","captured","marry","introduce",
"salvation","dc","intelligent","portrait","bills","sam","button","stick",
"differential","sustained","import","invasion","strict","angeles","partnership","patent",
"paint","frontier","log","preference","belongs","clark","bus","enthusiasm",
"fewer","exclusively","seeds","concentrations","sovereign","signals","virus","specimens",
"australian","rays","lessons","calm","technological","yours","inclined","arrive",
"players","crystal","parish","driver","dimension","tip","bitter","savings",
"tender","temporal","muslim","video","privilege","charter","massive","promotion",
"arose","worn","responsibilities","qualified","adopt","illustrate","weekly","prevented",
"camera","foods","bent","engage","ending","vain","conflicts","manufacture",
"folk","mystery","operated","celebrated","emotion","seconds","chemistry","romantic",
"prevention","manuscript","allies","dates","margin","qui","bush","morality",
"replace","urged","avoided","anyway","occasional","inter","tissues","graduate",
"controversy","inventory","illustrations","joe","bulk","greece","admission","sending",
"relate","mineral","mercy","polish","anne","forever","calcium","ft",
"consisted","commissioner","improvements","fame","varies","arises","santa","plenty",
"thermal","respects","medieval","slope","refuse","risks","complexity","innocent",
"protestant","continent","gentle","describing","maria","soc","ignorance","trials",
"encountered","conceived","closing","tobacco","crew","sisters","adoption","fraction",
"korea","wore","reward","protest","choices","na","employers","pulse",
"proposals","artistic","compelled","widespread","merchant","operator","expanded","researchers",
"classic","topics","analyses","wherever","marginal","consist","wives","defend",
"inspection","enabled","requiring","adjacent","ritual","pressures","bare","simon",
"poland","digital","furniture","nitrogen","atoms","partially","engineers","wings",
"ne","egg","reporting","devil","sensitivity","ford","blacks","merchants",
"mounted","monitoring","finish","incorporated","quietly","dans","tape","survive",
"libraries","senses","das","spatial","ben","registered","yard","breakfast",
"clerk","disposition","revised","sing","gravity","margaret","maps","anywhere",
"jane","segment","bath","circles","painful","humans","tract","fruits",
"personally","pound","mortality","consumers","explains","courtesy","laugh","au",
"obtaining","headquarters","filter","publishers","monetary","actors","boats","clouds",
"tour","poets","experts","intent","blessed","tomorrow","extending","regards",
"forests","employ","metals","decreased","coverage","gulf","atomic","worry",
"mines","bears","restored","imports","cloud","excitement","roosevelt","midst",
"furnished","dialogue","fees","apartment","interference","stem","phases","opera",
"throat","tended","push","arising","portions","census","succeed","gains",
"covers","merit","trail","deemed","indirect","reliable","retain","engineer",
"losing","investigations","array","inflation","uncertainty","seized","belonged","settle",
"colleges","pan","realm","favorite","andrew","lords","platform","player",
"revelation","shoes","rubber","fears","acquire","neighborhood","fragments","symbolic",
"wales","designated","saturday","prescribed","electricity","daughters","proceeds","optical",
"stuff","immense","aunt","bottle","seventh","affecting","austria","organisms",
"russell","presumably","indicating","jim","vector","valve","anderson","bore",
"yesterday","loop","priority","computers","placing","prospect","beds","retired",
"recommendations","absorbed","vietnam","georgia","believes","passive","ideology","bibliography",
"favorable","delight","pairs","natives","construct","visitors","isolation","wondered",
"frederick","bacteria","boards","im","divorce","hamilton","acknowledged","loud",
"dynamics","varieties","delicate","guidelines","mix","remarked","parameter","allied",
"faster","holes","indication","origins","powder","committees","linguistic","precious",
"lifted","stimulus","serum","solar","calculation","facility","banking","retirement",
"chapel","ha","notwithstanding","controlling","franklin","encounter","jersey","relatives",
"legitimate","authorized","dominated","inherent","arrest","odd","eighteen","crossing",
"transformed","bands","adds","petition","napoleon","flag","recovered","howard",
"justify","fox","disk","occurring","pen","reveals","seventy","km",
"outstanding","attracted","puts","automatic","mayor","prosperity","taxation","insight",
"explicit","descriptions","memories","reaches","creates","remainder","cable","constituted",
"whites","res","max","comparable","inadequate","uk","advances","chem",
"solely","unlikely","creature","terror","hostile","gardens","motives","albert",
"nodded","resident","ranks","transmitted","burned","integrity","ceased","witnesses",
"pronounced","alleged","pole","basin","ff","physicians","classified","tribe",
"enforcement","lunch","behavioral","followers","intimate","deliver","guilt","rs",
"continually","resist","venture","incidence","confused","heating","collect","definitions",
"aims","constraints","diverse","commons","reflects","foundations","gently","companion",
"ignored","schemes","fatal","mills","diffusion","implied","performing","latest",
"visits","lesions","ruling","elderly","creatures","moses","pollution","gathering",
"elaborate","angles","crimes","quarterly","undertaken","programming","mississippi","turkey",
"gallery","bases","eq","clergy","killing","referring","scales","douglas",
"specimen","excluded","receives","fourteen","restoration","votes","divide","holland",
"reasonably","compromise","silk","developmental","loving","glance","attachment","dedicated",
"reforms","exceptions","attained","generate","ties","pursuit","fired","une",
"deposit","uncertain","detection","appeals","highway","dried","tho","semi",
"photo","menu","peasants","surplus","nucleus","usage","spaces","prisoner",
"successive","remedy","manufacturers","cord","myth","titles","theological","substitute",
"hate","moore","buying","administered","solving","senator","stroke","salary",
"farms","hospitals","cd","fiscal","literally","pace","autumn","bulletin",
"cure","proceeding","tight","xi","aimed","stepped","regiment","supervision",
"tumor","carrier","historic","automatically","theater","unconscious","breathing","prophet",
"photographs","guarantee","completion","artery","fancy","sur","hitler","es",
"exercised","borne","ruled","starts","edges","asks","sci","remark",
"pupil","hide","alter","initiative","calculations","pilot","cultivation","arrested",
"lesser","mild","tank","lectures","settings","approaching","continental","commonwealth",
"award","stranger","surgical","timber","assignment","improving","defects","tragedy",
"generous","complaint","ceremony","sixteen","rely","evaluate","dimensional","lecture",
"investments","voyage","suspected","marshall","mrs","grants","cooperative","configuration",
"fails","damages","wells","bars","attempting","hebrew","exceed","bow",
"gates","fitted","comparatively","egyptian","outcomes","healing","functioning","conquest",
"submit","lovely","emphasized","genes","sorts","territories","subjected","declined",
"kennedy","nearby","observer","requested","arbitrary","hunt","switch","springs",
"objection","scripture","infants","feedback","entity","privileges","guests","roughly",
"blame","governing","supports","peasant","urine","collections","fur","tales",
"madame","seal","constitutes","reconstruction","distress","triumph","ions","giant",
"th","quantitative","foster","adjusted","seated","mad","cream","appreciation",
"definitely","parliamentary","challenges","communicate","charity","loyalty","presently","brazil",
"rotation","pursued","separately","yields","default","carries","retreat","escaped",
"impulse","missionary","pursue","lift","burst","proc","sleeping","substantially",
"interviews","neglect","flood","motivation","lane","lawyers","hanging","motive",
"correctly","nerves","upward","assembled","clock","feared","associates","deck",
"sooner","explore","ing","wisconsin","mighty","neglected","themes","seats",
"reprinted","islam","grows","shorter","availability","armies","beer","movie",
"farming","anglo","knife","mature","argues","technologies","somebody","recalled",
"sampling","preface","dancing","badly","ridge","alternatives","preservation","defeated",
"liberation","rigid","surrender","archives","manners","mobile","criterion","hitherto",
"associate","deficiency","lens","reception","publisher","pointing","novels","ct",
"rebellion","excuse","li","grammar","abundant","manifest","earned","appreciate",
"hi","deputy","historian","commands","commanded","organism","informal","lover",
"certificate","imagined","bob","pakistan","aristotle","detected","jefferson","everyday",
"keeps","invention","politicians","sociology","belt","bias","perfection","gordon",
"dad","sentiment","riding","protocol","specialized","lung","races","responded",
"photograph","suspended","suicide","voting","renal","lots","rises","visiting",
"checked","impressed","capitalism","registration","managing","demonstration","sheets","baltimore",
"static","behaviors","prize","pitch","immigration","capitalist","listened","variance",
"wright","justification","perceive","handsome","singular","tie","immigrants","acknowledge",
"subjective","federation","bold","socialism","filed","landed","sessions","contacts",
"filling","unnecessary","stimulation","eager","dawn","princess","presidential","streams",
"pine","electrons","producers","pa","physiological","kg","combat","princeton",
"inspiration","butter","geographical","nearest","flexible","knees","netherlands","meanings",
"expenditures","landing","wedding","sizes","package","angel","historians","dean",
"perspectives","suspect","marx","recover","smell","abraham","cents","governmental",
"thompson","sweden","soils","paintings","accurately","thereafter","sport","zealand",
"boxes","rod","online","verb","modification","locations","assess","aggressive",
"lakes","bishops","advised","balanced","aggregate","preparations","sensation","romans",
"combine","wash","rat","talent","erected","attribute","grief","premises",
"benjamin","pulmonary","missouri","sins","seventeenth","enters","winds","grades",
"integral","addresses","inevitably","predicted","condemned","prayers","junior","elevated",
"flame","suspicion","styles","injuries","peaceful","competent","pump","disposal",
"reproduction","illustrates","morgan","admiral","promises","matthew","innovation","deals",
"lacking","illegal","wanting","distinctive","calculate","mercury","enzyme","combinations",
"disposed","thrust","rolled","brand","governed","dictionary","proportions","centres",
"finite","fibers","actor","irregular","carriage","rolling","vehicles","tubes",
"extends","extract","aesthetic","cylinder","replacement","subtle","restaurant","unfortunate",
"eliminate","termed","saints","virgin","edinburgh","sarah","devotion","okay",
"accustomed","biology","affects","shame","pp","eighth","label","jacob",
"prolonged","sketch","segments","declare","plains","resort","secular","abilities",
"violation","hierarchy","rush","potentially","repeatedly","statutes","leather","battery",
"counties","restore","vienna","threshold","initiated","em","brave","commissioners",
"lease","algorithm","sovereignty","autonomy","mouse","node","disaster","simplicity",
"slip","skilled","cousin","corruption","reserves","tropical","cardinal","estates",
"injection","ideological","horizon","norman","naked","prejudice","suggesting","elevation",
"grateful","loaded","whereby","walker","professionals","businesses","cardiac","artillery",
"alfred","shelter","occupational","infantry","brick","tide","cultivated","examining",
"certainty","metropolitan","shapes","ambassador","accomplish","accumulation","operative","permits",
"injured","truck","chiefs","diary","explicitly","cheap","las","craft",
"tremendous","smoking","dissolved","freud","barely","modest","ve","sharply",
"optimal","doubtless","households","monitor","nights","respiratory","virtues","fiber",
"plato","abnormal","coastal","moisture","amino","penalty","negroes","directory",
"nurses","renaissance","discussing","tin","despair","seeks","imported","ignorant",
"comparing","physically","norms","notions","wake","spinal","insects","ego",
"obedience","geography","searching","barrier","palm","splendid","shops","mud",
"shade","continuity","patience","decree","delhi","ports","rejection","reproduced",
"facilitate","financing","vegetables","harris","sectors","disputes","numerical","decay",
"intensive","vitamin","judgments","blank","resolve","chains","greeks","xii",
"shipping","anticipated","anna","defining","victoria","phys","cavity","morris",
"thirteen","ln","backward","cavalry","guided","teams","broader","biblical",
"washed","gather","adaptation","gaze","turkish","er","anybody","elder",
"drawings","fix","sympathetic","disturbed","mobility","limitation","neighbors","differently",
"emerging","hunter","happening","missed","robinson","dare","magnificent","heritage",
"grasp","corresponds","warfare","therapeutic","islamic","lifetime","diagnostic","glorious",
"utterly","peripheral","rows","herbert","elite","thorough","currents","guest",
"entertainment","relates","void","angels","luke","exploration","kansas","occupy",
"conceptual","piano","exhibition","fold","cease","prose","suited","pink",
"traits","knight","beloved","memorial","license","ranging","multi","compliance",
"knee","planes","circumstance","complaints","adverse","outward","berkeley","scottish",
"useless","choosing","stared","editors","campbell","indigenous","pot","leisure",
"florence","cathedral","assurance","realistic","independently","identifying","inserted","stocks",
"diplomatic","philosopher","dull","luck","collapse","gradual","ore","judged",
"com","cooking","harbor","settlements","paths","economies","vein","strip",
"severely","rival","withdrawal","radius","enables","differentiation","abundance","pity",
"attending","unexpected","asset","deviation","deed","grandfather","investigated","baker",
"installed","surveys","enhanced","reminded","safely","mysterious","sa","survived",
"delayed","eighty","expand","hindu","laughter","heated","funding","guards",
"expressing","nelson","lamp","imply","sensory","burn","handled","renewed",
"exhausted","distances","widow","monday","eve","protective","acquainted","programmes",
"crowded","sorrow","supra","analytical","pi","versions","inheritance","correction",
"pressing","ce","humble","planted","codes","defect","zones","philosophers",
"apt","monopoly","assisted","cruel","temper","factories","warned","presenting",
"proportional","representations","steadily","ideals","assumes","equipped","goodness","beneficial",
"expectation","brush","roger","nearer","influential","sensible","prospects","funeral",
"graph","mortgage","feminist","friday","descent","rhythm","grains","stairs",
"asserted","engines","atom","poles","spontaneous","vigorous","festival","princes",
"regulatory","ranges","editorial","toronto","mediterranean","laser","vague","reviews",
"apple","dangers","regression","speeches","intentions","traces","marie","portuguese",
"shallow","respondents","shirt","offensive","coefficients","exhibited","ambition","lo",
"bowl","doctrines","bits","debts","territorial","considers","irrigation","chloride",
"scholar","breach","operational","displays","spots","mit","deaths","makers",
"invisible","interpret","esteem","annually","admiration","consistently","explaining","settlers",
"indiana","crude","breeding","decisive","chances","oak","drops","winning",
"colorado","poetic","explanations","weapon","catholics","painter","meaningful","deeds",
"specify","swept","wool","ghost","molecule","warrant","legend","borders",
"secondly","enlarged","commodity","strikes","rats","nationalism","commentary","contest",
"displacement","revenues","prevailing","emergence","assault","int","poured","needle",
"phrases","orthodox","arnold","propaganda","mechanics","posts","utmost","doubtful",
"talks","sophisticated","curiosity","precision","covenant","disciples","emerge","notable",
"commodities","interrupted","oxide","reviewed","trunk","asleep","simulation","hong",
"mi","intercourse","nutrition","pension","gases","attendance","frozen","fool",
"potassium","tel","minimal","lion","pack","resting","engagement","counted",
"savage","flowing","cave","exclaimed","treating","handbook","wealthy","horror",
"ours","ignore","desperate","rode","oath","altar","recommend","thread",
"specially","actively","assertion","cooling","alarm","implemented","ancestors","romance",
"cuba","investigate","smiling","studio","sentiments","chicken","propose","locked",
"loading","ruin","ruler","shoot","discretion","virtual","realization","athens",
"interfere","correspond","opens","stretched","helen","ecclesiastical","appearing","planet",
"shooting","cycles","environments","metabolism","ny","whispered","cleared","promoting",
"consensus","deliberately","glucose","dismissed","maryland","laughing","oldest","floating",
"cerebral","torn","exciting","exclusion","carter","hatred","determines","intellect",
"offence","flexibility","rail","juan","harper","touching","archbishop","wit",
"hang","onset","evolved","commit","effected","computed","allocation","shaft",
"fled","alien","inability","accounted","perceptions","collecting","hired","decreases",
"coloured","minerals","globe","horn","para","alice","differs","refusal",
"eliminated","stresses","retail","selective","implement","harvest","develops","maturity",
"solved","orleans","rhetoric","stretch","traced","blessing","viz","publicly",
"encouragement","fabric","barriers","sail","flux","unhappy","xiv","impressive",
"accessible","swift","rings","rid","tolerance","sixteenth","macmillan","ac",
"cluster","worried","contempt","journals","critique","passengers","assure","removing",
"successor","politically","marble","clubs","reliability","relieved","aggression","providence",
"dense","shed","fatigue","concentrate","openly","tokyo","lodge","reject",
"induce","korean","veins","analyzed","overseas","complained","frequencies","complications",
"acquaintance","amended","compression","palestine","valued","seas","infections","bernard",
"heading","spare","recognised","congregation","tenth","critic","energies","vacuum",
"accepting","keys","ross","breaks","whence","invariably","joints","cheese",
"kong","analogy","cuts","xiii","susan","formulation","barbara","manufacturer",
"confident","enhance","rulers","conjunction","madison","destiny","reduces","hardware",
"manufactured","deposited","operators","emphasize","limiting","fracture","receptor","nonetheless",
"nowhere","inform","attain","kiss","dirty","believing","threatening","omitted",
"shadows","sequences","ate","vegetation","lock","outlined","furnish","vegetable",
"campus","obscure","cement","diminished","colours","undertake","reads","bleeding",
"investors","persistent","pains","distinctly","persian","northwest","immune","commenced",
"merits","chambers","lasting","scholarship","encouraging","entries","buyer","deprived",
"milton","promoted","fortunately","convey","rope","slide","warren","anthony",
"caution","bedroom","customary","producer","friction","evaluated","regret","lit",
"voters","similarity","ceiling","confirm","biography","verses","outlook","wilderness",
"expanding","opponents","connecticut","attract","dining","interpretations","ho","estimation",
"thence","pays","therein","launched","hungry","lt","russians","supplement",
"stuck","murray","lighting","gratitude","requests","observing","arrow","laying",
"kindness","victor","crying","stressed","petroleum","impose","packed","tries",
"mom","wolf","coupled","loves","socially","purity","coach","adventure",
"southeast","statutory","induction","weights","bench","lately","inherited","bronze",
"tennessee","readings","vocabulary","kentucky","surgeon","regulated","rests","projected",
"receiver","sounded","supper","ratios","communists","hunger","objections","revolt",
"juice","lime","voted","organize","drainage","convert","endless","fitting",
"earnest","automobile","junction","nodes","dual","basically","notably","persuaded",
"therapist","bargaining","pregnant","letting","shouted","instantly","truths","chancellor",
"conventions","median","invitation","selecting","butler","jump","deer","overview",
"missionaries","mistaken","destructive","minded","crystals","infected","derive","hollow",
"instructed","insufficient","kindly","minnesota","cow","dan","enacted","doses",
"rating","dies","shifts","hungary","subordinate","architectural","activation","agenda",
"favourable","underground","destination","reflecting","fertility","hay","disability","elastic",
"stern","harder","tribal","luther","companions","worlds","projection","purple",
"cabin","capabilities","advocate","coordination","doubts","allowance","quantum","consumed",
"acre","span","achieving","questioned","attraction","magazines","firing","popularity",
"connect","preferences","solemn","flour","disturbance","revision","tendencies","tariff",
"navigation","failing","bacon","payable","exploitation","impressions","ton","inn",
"spreading","hoping","iowa","termination","misery","tough","occupations","keen",
"flash","profitable","ab","glasses","harold","dublin","pause","newton",
"reinforced","targets","religions","superficial","cleaning","mistakes","pierre","congressional",
"mixing","camps","denial","lungs","shifted","stimuli","organizing","switzerland",
"hostility","preventing","humor","eminent","earn","superiority","mask","missions",
"tonight","cal","democrats","script","predict","exit","chemicals","turner",
"jordan","trustees","delegates","competence","roy","lasted","finest","nest",
"invested","residential","abandon","comparisons","nicholas","purchasing","biol","timing",
"strains","louisiana","heights","brethren","muslims","honey","stanford","specification",
"seemingly","speakers","implicit","smallest","arrange","meets","muscular","incidents",
"catherine","haven","privacy","psychiatric","bloody","accompanying","episode","unpublished",
"accommodation","accord","passions","respected","repetition","pin","undertaking","lest",
"yale","vascular","mice","maternal","bodily","tomb","grid","severity",
"aging","tracks","bride","forbidden","detect","tion","fond","formally",
"indirectly","tumors","conducting","founder","prediction","wheels","afterward","throwing",
"relaxation","dwelling","fires","threats","zinc","bid","churchill","depths",
"instinct","approximate","coalition","citizenship","damaged","brass","invented","enjoyment",
"cooper","reversed","scheduled","faint","charm","suspension","exceptional","carl",
"ample","checking","loyal","consistency","kidney","sermon","boiling","depressed",
"chase","superintendent","accumulated","councils","disappear","cellular","incomplete","upset",
"restriction","introducing","karl","antonio","rounded","coarse","likelihood","abdominal",
"gesture","shopping","respecting","inputs","witnessed","comedy","borrowed","traditionally",
"catalogue","assert","steep","geometry","wherein","hon","donald","philippines",
"frightened","centered","marched","midnight","overhead","refuge","challenged","summit",
"demanding","stanley","obey","crack","strangers","shells","architect","treaties",
"patrick","stuart","meals","preaching","dental","guinea","adjust","administrator",
"installation","dialog","bridges","ninth","holiday","brigade","promising","sickness",
"tooth","eaten","smart","parker","ordinance","confession","convenience","compact",
"shri","vicinity","insist","entities","downward","ecological","treatise","graham",
"counts","depended","sums","feast","expresses","transform","maker","washing",
"calendar","seasons","achievements","wounds","rage","pulling","amsterdam","iraq",
"lucky","connecting","tragic","reflections","dressing","deficit","hormone","burns",
"lowered","descriptive","seller","europeans","hudson","caste","thirds","meantime",
"stewart","protecting","noon","insert","exile","oregon","parental","rescue",
"forum","fellows","heroic","heavenly","reformation","defines","ruins","jew",
"khan","draws","ss","afforded","charming","eu","accidents","electoral",
"grandmother","passenger","strengthen","mistress","emission","civilian","diamond","volunteers",
"routes","quote","sustain","rehabilitation","enclosed","strongest","utilization","fortunate",
"idle","sexuality","possesses","wicked","counting","positively","principally","evans",
"substitution","signature","bladder","intrinsic","miserable","continuously","pile","adequately",
"assessed","paradise","listing","te","pig","utilized","pit","tent",
"bigger","flew","fraud","longitudinal","supporters","concert","joining","heroes",
"shake","rushed","revival","troubled","corrected","inclusion","kid","troubles",
"talents","sandy","slept","promptly","manchester","ralph","lightly","indications",
"momentum","leaned","shifting","trauma","traveling","administrators","prestige","distributions",
"zeal","awful","peer","intake","opposing","residual","amplitude","conceive",
"communism","capability","circuits","spouse","lloyd","editions","poorly","methodology",
"buffalo","modify","spin","raises","multitude","soluble","nominal","drag",
"boss","genus","contexts","glands","glanced","morrow","dioxide","venice",
"cope","embrace","favored","composite","garrison","collaboration","zu","trap",
"fred","iran","absurd","drunk","failures","mike","swimming","anatomy",
"fe","modifications","diabetes","ou","sink","buffer","periodic","singh",
"prevailed","discharged","chairs","placement","detroit","frames","plainly","football",
"tenant","consult","peninsula","treatments","ot","fulfilled","slipped","yeah",
"defensive","hated","convince","checks","rocky","richmond","pastor","lonely",
"creator","delighted","deceased","exceedingly","deposition","kant","jail","tanks",
"singapore","gear","securing","counseling","visitor","toxic","observers","scriptures",
"ruth","consultation","rio","foreigners","obstacles","workshop","qualitative","retention",
"intact","satellite","analyze","depreciation","super","contraction","quest","feminine",
"gregory","fragment","elegant","deliberate","treasure","debates","competing","overwhelming",
"printer","guardian","stimulated","carriers","crazy","oliver","extensively","vulnerable",
"recreation","tactics","stating","southwest","caesar","statue","hugh","sincere",
"cargo","conferences","travels","destined","prohibited","geneva","tear","elimination",
"inflammation","bruce","arbitration","belgium","addressing","excellence","bengal","panic",
"performances","bark","harrison","analogous","ongoing","limbs","drives","conspicuous",
"utter","temporarily","sang","arriving","wrapped","collar","withdraw","dynasty",
"stupid","bomb","forehead","civic","miracle","enforce","airport","advisory",
"temples","travelling","imitation","evolutionary","scarce","babies","jan","backed",
"propositions","prosecution","harsh","confronted","squares","costly","hers","salaries",
"exceeding","lined","swedish","prey","proclaimed","preceded","abortion","metaphor",
"seventeen","warmth","oxidation","qualifications","fan","foolish","illusion","ist",
"recommendation","urge","traders","staying","juvenile","hereafter","phosphate","yielded",
"baron","ambitious","specifications","receipt","lamb","histories","motions","sage",
"ltd","figs","causal","shear","withdrawn","scattering","corners","incapable",
"amid","chin","fierce","avoiding","invest","sp","delta","favourite",
"indicators","computing","anthropology","owe","disturbances","coin","bend","sculpture",
"wiley","siege","concealed","porter","appreciated","burial","alpha","lively",
"cleveland","structured","drift","guides","potatoes","urgent","literacy","relevance",
"triangle","scientist","mitchell","cortex","denmark","alan","dividing","austrian",
"ordinarily","clearing","approximation","hiv","monument","beaten","modeling","prospective",
"neighbor","tribute","expertise","possessions","peers","geological","quit","regulate",
"jumped","willingness","freight","manifestations","rested","infrastructure","pushing","accompany",
"dick","cottage","carved","hut","austin","resistant","risen","portugal",
"vocational","contradiction","distinctions","sect","fax","adolescent","sydney","quebec",
"speculation","lip","sailed","extraction","beef","shield","erosion","norway",
"manifested","evaluating","conveyed","loads","ninety","deserve","isaac","stir",
"oriental","skull","documentation","ontario","inference","persuade","combining","shaking",
"fusion","historically","walks","transverse","nuclei","screening","funny","swing",
"stalin","forgive","sergeant","salts","refugees","canon","pas","instrumental",
"resurrection","rewards","reliance","poison","synthetic","imprisonment","govern","commanding",
"sociological","luxury","christopher","faults","purchases","prohibition","stake","confess",
"bacterial","communion","conversations","labeled","stops","rightly","buddhist","parks",
"operates","antiquity","biggest","fence","physiology","breakdown","awarded","mason",
"degradation","alongside","nationalist","dam","atmospheric","devised","dissolution","inequality",
"incurred","indifference","bourgeois","tribunal","gland","blade","alert","ok",
"arizona","climb","descended","nazi","shining","enzymes","incomes","realities",
"ceremonies","submission","demonstrates","birthday","maintains","buddha","advancement","economically",
"injustice","oppression","battles","coins","guaranteed","summarized","departed","exclude",
"chaos","dating","limestone","disappointed","texture","efficacy","tones","restraint",
"neighbourhood","ordering","clinton","revenge","equals","boots","defended","bombay",
"monks","strengthened","discount","inland","permanently","stopping","motivated","clever",
"confirmation","prophets","liquor","cheek","publicity","spaniards","publish","shaw",
"struggles","meters","indicator","xv","receptors","remind","ash","indispensable",
"aluminum","compete","casting","pc","dominion","angular","sized","slightest",
"nancy","intend","slopes","campaigns","monarchy","indies","explored","gaining",
"hurry","psychiatry","trustee","locate","dorsal","fisher","gradient","oppose",
"hypotheses","staring","corpus","alternate","matching","prompt","couples","roberts",
"reporter","formidable","standpoint","paused","alabama","petty","clin","amendments",
"strings","memoirs","trusted","hurried","baptist","beg","mentally","wallace",
"simplest","broadcast","filing","je","nm","limb","stamp","spine",
"react","presidency","indifferent","flies","pop","surprisingly","advancing","organisations",
"israeli","proves","snake","substrate","behold","refined","solomon","graphic",
"surviving","lengths","incentive","hopkins","deciding","resolutions","pipes","inconsistent",
"decorated","pencil","gilbert","theorem","fantasy","professors","imaginary","resemblance",
"nobility","goddess","pleasures","delightful","behave","lovers","apostles","spell",
"binary","embedded","secretion","os","dirt","republicans","xvi","heavens",
"agreeable","realism","enforced","ussr","meter","authentic","orchestra","descendants",
"climbed","disc","respectable","fuller","softly","rolls","furnace","vivid",
"cared","perpetual","negotiation","discoveries","hint","collector","reed","plots",
"generalized","baptism","solitary","suite","concludes","burnt","evils","offset",
"constituents","foregoing","heir","communal","stems","specialist","depicted","columbus",
"straw","planting","manuscripts","drain","dish","inhibition","clusters","claiming",
"guys","admired","rooted","examinations","mortal","locally","clauses","punished",
"spectra","beast","singer","legally","uncommon","suits","transparent","tips",
"hire","bending","caribbean","shareholders","relieve","susceptible","advocates","tunnel",
"cult","dilemma","relied","fertile","phillips","weary","exhibits","stiff",
"trains","practitioners","coordinate","incentives","arabic","husbands","alteration","happily",
"bind","nigeria","hans","echo","aroused","insect","beans","lean",
"fits","domination","temptation","memorandum","viewing","soap","twin","railways",
"maine","noting","substituted","offspring","rendering","olive","transfers","likes",
"tightly","watson","defendants","remarkably","trips","valleys","arabs","struggling",
"particulars","rogers","indonesia","marital","rape","wrought","wondering","basket",
"imaging","ip","conceptions","remedies","focusing","renewal","hook","pardon",
"jonathan","fourteenth","homogeneous","constructive","augustine","secrets","thumb","simultaneous",
"collins","latitude","marriages","sheer","tourism","partition","vapor","autonomous",
"imagery","stimulate","tune","holder","geographic","movies","jacket","nay",
"whatsoever","surroundings","lesion","formulated","premium","gotten","polar","specialists",
"numbered","leo","preached","cubic","solvent","duly","merchandise","lend",
"melting","plantation","beating","neural","officially","edwards","capita","swiss",
"nixon","charlie","relaxed","disclosure","univ","coil","controversial","complement",
"pioneer","conspiracy","originated","wagon","lightning","semantic","trinity","messenger",
"deserted","antibody","impulses","ali","eagle","kent","puerto","accommodate",
"execute","rifle","app","employing","gang","deduction","rna","orbit",
"balls","adjustments","explosion","charlotte","comprehension","tenants","exposition","fluctuations",
"jet","casual","resemble","forgot","conquered","drum","anchor","pet",
"pepper","sheriff","civilized","portfolio","righteousness","metabolic","membranes","cries",
"aided","imperfect","awake","insulin","proclamation","ink","traveled","canvas",
"billy","disabled","ticket","bass","recognizing","kissed","contractor","cups",
"sung","tense","consulting","genesis","generating","trades","endure","cohen",
"conflicting","honourable","harbour","query","differing","fibres","agitation","ecology",
"damn","reluctant","cir","expressly","prevents","manifestation","taiwan","succeeding",
"celebration","enjoying","appealed","saxon","slender","convincing","marshal","fashioned",
"arithmetic","dissertation","ascertain","suppression","melancholy","resigned","disappointment","perpendicular",
"systemic","container","hardy","enthusiastic","hungarian","maid","bankruptcy","hiding",
"charts","inscription","comic","delegation","cannon","drying","upwards","participating",
"treasurer","cake","bags","jose","mate","inspector","meditation","legacy",
"whoever","reagan","pottery","pearl","answering","oneself","beta","ingredients",
"che","symmetry","madrid","telegraph","founding","syria","mapping","java",
"westminster","compatible","obstruction","upright","fatty","facial","minimize","transit",
"sunlight","hughes","summoned","shelf","feasible","figured","manipulation","forcing",
"individually","conductor","contributing","sultan","ins","liabilities","neurons","universally",
"consequent","kate","pat","battalion","initiation","impaired","persecution","militia",
"lacked","maritime","constraint","embodied","governors","ugly","amounted","faculties",
"implication","custody","countryside","owed","peaks","generals","vocal","variability",
"twelfth","constituent","peru","arrows","dim","inward","assessing","confederate",
"anonymous","hereditary","curved","insights","hip","exchanges","designer","unwilling",
"diego","conformity","clinic","creditors","supervisor","interventions","rebels","documented",
"dominance","cheeks","volunteer","graduated","aspirations","caring","breadth","beams",
"reinforcement","quotation","entertained","commented","sailing","baseball","enlightenment","hr",
"dishes","cows","decreasing","closest","countenance","cigarette","auto","swelling",
"admirable","calcutta","eric","victorian","seasonal","scenario","commencement","chile",
"challenging","winding","knights","drill","broadly","creativity","consulted","neighbours",
"tenure","sealed","generic","sue","minorities","earthly","metallic","noun",
"reproductive","rabbit","apostle","monarch","locality","defective","shores","fearful",
"conform","opponent","fifteenth","assign","pt","discrete","kelly","unite",
"focuses","eligible","resonance","discusses","extracted","frustration","rites","communicated",
"optimum","mutually","vis","audit","isbn","focal","carcinoma","spencer",
"inclination","spelling","continuation","salmon","precipitation","pious","exaggerated","tuberculosis",
"stolen","alaska","practiced","envelope","conferred","landlord","archaeological","socio",
"deity","dividends","parking","magistrate","contingent","priorities","proven","hazard",
"propagation","freed","predominantly","revealing","exceeds","unpleasant","frost","una",
"executives","gothic","miscellaneous","folded","redemption","reservoir","declining","sends",
"mo","zur","procession","bet","cc","advise","economists","undergo",
"pastoral","solidarity","granting","screw","murdered","wires","railroads","hollywood",
"elect","resembles","greatness","commissions","plea","audiences","mentions","cliffs",
"ib","wagner","acta","complain","exchanged","aloud","exterior","adolescents",
"correlated","amazing","inflammatory","mt","disadvantage","matched","launch","ram",
"prussia","loses","imperative","dem","monastery","ruined","imposing","combustion",
"feudal","worldwide","dragged","ni","crushed","sri","compulsory","participated",
"preacher","dis","northeast","productions","contracted","investigators","cr","taxable",
"antibodies","tourist","warriors","paradigm","resumed","deserves","suppressed","sponsored",
"competitors","gandhi","probe","hull","questioning","immunity","turks","exceeded",
"compiled","conditioning","capacities","profiles","theodore","theirs","norm","resignation",
"deaf","appetite","joan","joke","ar","wax","layout","debtor",
"margins","adventures","pursuing","arteries","burke","appearances","monk","arguing",
"privileged","microsoft","morals","laboratories","pathology","habitat","posed","musicians",
"presumed","transient","abbey","prompted","advocated","portraits","cooked","prevail",
"comply","yearly","domains","basal","acceleration","breed","terminology","serial",
"weber","invite","refusing","activated","pond","cumulative","cone","sweat",
"compassion","folks","holdings","monsieur","teachings","prevalent","appoint","hotels",
"computation","medication","laura","elders","piety","holmes","pleasing","responding",
"po","interpersonal","unified","pigs","alterations","patch","edit","su",
"stationary","tony","bounds","horrible","eyed","lucy","malignant","drivers",
"judging","beard","chorus","contracting","framed","wandering","flames","dramatically",
"subsidiary","homer","welcomed","tuesday","conditioned","trick","fortunes","supernatural",
"sacrifices","genre","martha","exploring","instructor","fitness","viewpoint","herald",
"taxpayer","instability","hereby","saturated","broadcasting","intestinal","fellowship","dreadful",
"electrode","corrupt","verdict","stained","va","weighed","composer","penetration",
"constants","gauge","receipts","hemisphere","ashamed","bilateral","questionnaire","grove",
"soup","unstable","applicant","exempt","cane","sits","extracts","incorporation",
"dwell","deadly","antigen","polymer","characterization","picking","contradictory","lab",
"destroying","sensations","narrator","barrel","embassy","eternity","paragraphs","detached",
"altitude","abolition","remembering","visions","predictions","spheres","pretend","enabling",
"rebel","subsection","ridiculous","governance","reservation","concessions","observes","prevalence",
"lighter","creed","workmen","stimulating","readiness","webster","litigation","arterial",
"settling","overlooked","judaism","ut","declares","innocence","consolidated","odds",
"socrates","darwin","lawful","unfair","neighboring","satisfying","embraced","mac",
"eliot","myths","tis","concluding","practised","quasi","photography","acquiring",
"jealousy","graves","saddle","attach","immigrant","contemporaries","golf","passionate",
"cemetery","chronicle","incorrect","rainfall","preserving","cats","ammonia","buddhism",
"wavelength","dealer","prints","affections","jaw","expecting","resentment","erect",
"vitro","coronary","satan","metaphysical","valuation","cinema","incredible","disadvantages",
"thunder","livestock","donor","neighbouring","drank","rude","chi","spray",
"ventricular","abolished","drinks","monuments","argentina","ap","agrees","participant",
"bloom","additions","diagrams","shots","dairy","outset","affinity","sherman",
"designing","literal","neo","enlightened","symposium","displaced","module","strata",
"miracles","quarrel","judgement","audio","towers","feeble","liberties","raymond",
"simpler","sanction","lenin","fetal","thursday","irrelevant","boom","leaning",
"axes","posture","stack","contention","stretching","sauce","reconciliation","folder",
"microscope","blown","decent","nineteen","convicted","jacques","helpless","textile",
"announcement","shortage","ai","bis","montreal","disabilities","epic","ellen",
"coupling","reflex","formulas","distal","clarity","approve","translations","processed",
"suffice","hamlet","mediated","adapt","decomposition","fore","unlimited","dared",
"harvey","thyroid","ta","bottles","vested","declaring","ec","uncomfortable",
"urinary","lent","folly","foremost","imperialism","requisite","feathers","postwar",
"verbs","privately","als","researcher","packet","plague","credits","grip",
"hood","owen","fluids","graduates","endeavour","reverence","warrior","rom",
"bc","straightforward","cruelty","hey","dispersed","resume","usefulness","distinguishing",
"shoe","symptom","fortress","organised","similarities","daring","fury","nobles",
"packing","tempted","consciously","constantinople","oils","heirs","fascinating","climbing",
"reynolds","ti","tensions","prophecy","geology","closure","engaging","labels",
"torture","negligence","strengths","pose","conditional","honorable","fr","cervical",
"accidental","md","implementing","suspicious","micro","patron","exerted","twisted",
"reciprocal","intellectuals","narratives","criticized","harmful","abdomen","spectral","leonard",
"exert","nat","obstacle","houston","beasts","unemployed","apprehension","unjust",
"waist","directing","patronage","shocked","decoration","neat","nonsense","breathe",
"atlanta","seize","alas","levy","polished","steve","arena","stirred",
"lifting","penalties","offense","humour","unchanged","curtain","fountain","compositions",
"allegiance","clan","expose","generator","bo","outlet","linking","weaker",
"scholarly","vacation","plateau","endowed","continual","sweep","ci","coherent",
"editing","plaintiffs","maurice","charitable","glasgow","sailors","mess","transported",
"token","realised","nj","diffuse","establishments","bite","adjoining","professions",
"della","prayed","ernest","oven","sterling","curse","latent","hypertension",
"graphics","sexes","recipient","marxist","lofty","clarke","respondent","edmund",
"tenderness","saviour","wholesale","knocked","travelled","schooling","sinking","interact",
"wednesday","differentiated","justly","marking","appraisal","um","managerial","sketches",
"ether","gazette","emily","rectangular","gravel","aa","episodes","wasted",
"matt","hammer","bureaucracy","indebted","ambiguous","excluding","selfish","possessing",
"pricing","beginnings","stevens","vincent","sunshine","shepherd","auf","radial",
"consolidation","descending","teaches","fundamentally","linen","strengthening","violated","stirring",
"nationality","champion","update","locke","subsistence","pr","ladder","seq",
"granite","horace","aboard","plausible","panels","proximity","ascribed","dread",
"adaptive","rated","thirteenth","prosperous","mob","supremacy","ein","orderly",
"marsh","disturbing","exemption","milan","strips","compelling","insured","mao",
"fixing","auxiliary","dropping","silicon","richardson","quartz","worldly","porch",
"denote","sic","hybrid","bee","outbreak","africans","sabbath","innumerable",
"farewell","infancy","multiplied","tyranny","pathway","complementary","successors","buyers",
"complexes","preach","awards","brian","scan","squadron","unsuccessful","segregation",
"incorporate","attainment","lamps","initiatives","infinitely","simplified","null","oklahoma",
"psychic","coordinates","cosmic","bargain","potato","identities","wrath","borrow",
"carbonate","dug","liberals","suppliers","tracts","notation","cheerful","insertion",
"exp","irony","solids","villa","rim","lordship","lumber","nasal",
"attacking","weaknesses","breeze","roses","experiencing","masculine","pl","kenneth",
"recognizes","emerges","silly","extremity","pittsburgh","miners","ratings","disciplines",
"amusement","replacing","bennett","wildlife","processor","autobiography","progression","vanished",
"mar","blocked","veterans","withdrew","emancipation","uttered","pursuant","outlines",
"routledge","patriotic","sc","psychologists","coping","xvii","dispersion","instituted",
"sunk","chromosome","blanket","borrowing","analyzing","consul","wishing","freezing",
"premature","ave","hampshire","sighed","unequal","accelerated","honesty","crosses",
"daylight","resisted","famine","inappropriate","trigger","prone","carpenter","kim",
"rang","hosts","por","sermons","imaginative","annals","eugene","fairy",
"rituals","blast","hart","lap","ol","comprised","motors","yielding",
"identifies","physiol","sticks","floors","gram","vanity","restaurants","demographic",
"arctic","grouped","dealers","jealous","shaping","weekend","racism","liverpool",
"hazardous","educators","rationale","pockets","silently","shah","allocated","munich",
"detachment","separating","eldest","careers","retire","gastric","mysteries","danish",
"hormones","proofs","advent","innovations","cliff","ally","martial","holders",
"halt","calculating","phil","gdp","expressive","damp","bells","interpreting",
"abruptly","wonders","denying","bless","unusually","terminated","ventilation","dos",
"emerson","sanctions","writ","blowing","presidents","architects","pm","divinity",
"uterus","standardized","spectacle","repairs","weakened","hist","ashes","congenital",
"sediment","dominate","ass","alex","shower","theoretically","robin","sprang",
"collision","negotiate","ellis","switching","believers","catalog","ascertained","potent",
"handful","doubled","xx","elbow","recurrent","chocolate","jungle","assessments",
"ban","delaware","norton","blake","pagan","diplomacy","protestants","assignments",
"controller","doorway","trembling","nickel","infectious","uniformly","deficient","backs",
"grabbed","repeating","efficiently","utah","supplying","interviewed","mama","dependency",
"copied","inquiries","intimacy","compute","thailand","proving","mcgraw","math",
"brooks","barn","sided","dietary","tastes","reverend","welsh","nephew",
"devote","esq","wang","murphy","mainland","emphasizes","overlap","cuban",
"surg","lighted","nuts","ammunition","portland","accepts","residue","successes",
"julia","distortion","turnover","marquis","mammals","adopting","affirmed","sustainable",
"jazz","regulating","lattice","breasts","transitions","sweeping","inasmuch","temperament",
"wretched","hawaii","syntax","xviii","constructing","appropriately","appointments","instructional",
"painters","kinetic","conversely","alexandria","offenders","convictions","quo","trait",
"attendant","tr","restrict","cautious","insignificant","substantive","finland","objected",
"contributes","misleading","penny","quotations","glow","gifted","pamphlet","doth",
"paradox","papal","blessings","correlations","cholesterol","youngest","triple","praised",
"affirmative","vibration","mastery","inquire","commissioned","utilities","jimmy","grounded",
"montgomery","derivative","resultant","terrorism","planets","morally","leon","violet",
"eagerly","riches","punish","headache","cornell","compass","fisheries","precedent",
"penetrate","respiration","township","ref","auditory","systematically","barry","lacks",
"eine","indignation","tap","stance","hegel","sulphur","aqueous","clerical",
"introductory","valves","bundle","forthcoming","ronald","uniformity","outdoor","jay",
"righteous","normative","manufactures","crash","arabia","naples","freeman","petersburg",
"problematic","legitimacy","rods","mars","speculative","basement","spinning","criminals",
"papa","tim","colonists","darling","projections","retrieval","blows","lowering",
"magistrates","phosphorus","absorb","derivatives","mystical","sunset","dorothy","disastrous",
"interstate","envy","consuming","moist","flora","mainstream","irritation","elasticity",
"greeted","crowds","bathroom","subsidies","slower","intuition","certificates","pending",
"struggled","comprehend","rental","hast","guiding","ir","confrontation","conceal",
"hume","consultant","monster","elephant","spiral","learnt","viable","energetic",
"precipitate","pledge","scared","plotted","alternatively","weighted","madness","belly",
"differed","patriotism","countless","earthquake","robertson","deepest","nick","cage",
"anniversary","tab","reservations","posted","pertinent","demonstrations","panama","anymore",
"trivial","cart","powell","schedules","excitation","sublime","gm","commitments",
"fantastic","kin","offerings","ra","earning","reformers","archaeology","br",
"candle","predecessors","rents","dividend","hoc","dante","rabbi","countrymen",
"sidney","protested","ps","canyon","fare","doubted","avoidance","imprisoned",
"soda","chris","premier","aforesaid","gel","venus","enduring","unreasonable",
"plantations","fishes","youthful","resembling","scenery","herd","extinction","supplementary",
"survivors","regimes","cole","birmingham","ambiguity","prejudices","trucks","fog",
"multiply","interactive","veil","favoured","mounting","sufferings","algorithms","tops",
"importantly","purchaser","lending","packages","defending","prairie","transformations","tennis",
"embryo","confessed","easter","workplace","markedly","venous","finely","dances",
"politician","finishing","completing","guarded","depart","pretended","criticisms","retaining",
"buttons","fractions","directive","float","communicating","admits","equitable","forgiveness",
"provisional","crest","wrist","perry","relax","arisen","methodist","fractures",
"analyst","oppressed","revived","realizing","rev","crises","merry","tribune",
"encounters","patches","innovative","arkansas","johnny","jerry","rachel","lad",
"cairo","newman","fulfill","upstairs","advisable","accession","illumination","scanning",
"aristocracy","understands","comprising","justices","leap","albany","toxicity","detective",
"palmer","dig","hesitated","holidays","restless","laborers","glimpse","sally",
"threads","alignment","melody","tech","bounded","estimating","fork","initiate",
"ranch","dislike","journalists","fossil","weighing","undertook","filters","electronics",
"persisted","swung","immortal","qualify","profoundly","batteries","coding","gary",
"psychotherapy","medial","ieee","gaps","inclusive","reorganization","recognise","fl",
"injected","fans","larvae","nursery","begged","hopeless","integer","faded",
"etc","secretly","trapped","hazards","clearance","paralysis","fidelity","cincinnati",
"precautions","shouting","theatrical","practitioner","analytic","exploit","intersection","brussels",
"cognition","deviations","nato","hunters","fixation","impacts","endeavor","colon",
"underneath","vectors","deformation","julian","administer","awkward","chamberlain","suffrage",
"abnormalities","vitality","contrasts","epoch","emma","picturesque","deserved","revolutions",
"contradictions","proliferation","lengthy","convergence","caroline","heavier","joyce","longing",
"vietnamese","personalities","louise","derives","presbyterian","statesman","detector","hypothetical",
"accent","deputies","reversal","knock","asserts","cor","providers","translate",
"distorted","persistence","certified","sank","duct","alloy","notices","trusts",
"touches","cheaper","holiness","duncan","bullet","symphony","ibm","radicals",
"blew","lastly","ranged","mandate","inhabited","bureaucratic","sanctuary","sullivan",
"recourse","sigh","mss","parade","searched","comrades","bred","ensuring",
"nails","cured","issuing","optic","vomiting","cum","chip","augustus",
"intestine","photographic","goethe","wesley","abstraction","frontal","bosom","characterize",
"contemplated","alloys","gestures","sincerely","clarify","occupies","bristol","commanders",
"sediments","assistants","kenya","nebraska","renders","costume","treason","persuasion",
"booth","compressed","beautifully","eleventh","appropriation","improper","hearings","vengeance",
"constrained","mediation","misfortune","refuses","doc","englishman","minus","magnesium",
"muhammad","neutrality","westward","introduces","byron","recruitment","confidential","eng",
"utilize","sliding","otto","discovering","herein","daddy","bees","impairment",
"violations","designation","contamination","pie","catching","nitrate","rupture","presumption",
"localities","thomson","contemplation","hierarchical","spotted","colleague","bother","cu",
"frustrated","fashionable","rhetorical","grande","dragon","logs","marrow","bunch",
"thai","repression","admire","preferable","shades","formations","boiled","ranking",
"wills","betrayed","percentages","skeleton","guarantees","sierra","bowed","polite",
"paste","praying","moderately","forecast","probabilities","discomfort","celebrate","liberalism",
"boiler","prussian","quotes","dedication","verlag","discourses","kinship","permitting",
"meyer","ere","shanghai","undesirable","logically","suppress","amer","flock",
"theorists","duchess","scrutiny","entrusted","ferry","insure","enlargement","diminish",
"scots","heels","furious","extremes","photos","tag","gracious","longest",
"sterile","spared","strangely","punch","anticipation","poisoning","terrace","matches",
"weigh","graceful","prescription","speeds","ferdinand","bat","senators","brow",
"seizure","psychologist","coup","canals","subscription","awakened","episcopal","gasoline",
"tiger","horns","spectator","brains","indices","noticeable","localized","applicants",
"alcoholic","carlos","grouping","clearer","individuality","marcus","racing","nutritional",
"arrives","exquisite","aboriginal","av","amusing","johnston","tours","sexually",
"strand","supportive","lemon","coating","performs","reproduce","contrasted","adolescence",
"compensate","regeneration","spectacular","travellers","hesitation","ordained","charging","capillary",
"invalid","viscosity","hid","larry","shrine","ripe","volcanic","condemnation",
"nomination","reformed","wayne","lawn","crystalline","hospitality","prudent","pulp",
"pathways","negotiated","edwin","astonished","exploited","dame","reactor","decides",
"merger","clerks","cares","startled","epidemic","villagers","sich","acad",
"naming","melt","spark","monroe","albeit","steal","fibre","dakota",
"vicious","tan","mansion","reluctance","advantageous","rousseau","conservatives","sincerity",
"swallowed","hastily","dissolve","bestowed","macro","alarmed","ignoring","fulfillment",
"virtuous","franchise","celestial","lymph","carpet","hesitate","thinkers","mat",
"cromwell","scotch","bean","metaphysics","trout","joshua","transmit","digest",
"thoughtful","converts","denotes","reinforce","morphology","saturation","stripped","spherical",
"embarrassed","johns","inquired","vigorously","ment","ivory","qualification","explanatory",
"accomplishment","inflicted","install","memorable","pitt","carol","sore","instincts",
"dome","proximal","deterioration","persia","magical","negligible","nutrients","textbook",
"monkey","occupying","appealing","pertaining","invaded","marching","terrain","jamaica",
"ser","correspondent","marker","unaware","jar","sober","foul","swim",
"recalls","tickets","theft","shaken","strive","ascending","exhaust","emotionally",
"fastened","journalist","variant","storms","dreamed","tangible","melbourne","balances",
"traveller","dock","julius","bye","frankly","labourers","mist","plural",
"jointly","vulgar","honored","regiments","reminds","verify","reporters","nos",
"mourning","annie","optional","vacant","journalism","poorer","inverse","versa",
"lb","bailey","seattle","nous","pathological","linda","downtown","anticipate",
"collateral","quod","nicht","viruses","shelley","foliage","locus","moss",
"slaughter","surrendered","overnight","lining","bankers","provider","incidentally","yugoslavia",
"waved","restrictive","laden","creditor","macdonald","crust","investigating","plug",
"delays","surveillance","oval","voluntarily","tracking","rigorous","radically","mandatory",
"cicero","plaster","urging","axial","geometric","technically","disagreement","truman",
"amidst","malaysia","convent","entertain","persist","ba","additionally","dickens",
"threaten","assemblies","substituting","avail","legends","animated","hygiene","alkaline",
"counterparts","corridor","continuum","gambling","decorative","sometime","anal","shipped",
"winston","holt","supposing","dashed","pedro","italians","crowned","gabriel",
"flung","assimilation","gamma","contend","wireless","perceptual","occasioned","alienation",
"forestry","lever","economical","rewarded","swear","shrugged","twins","evaporation",
"toes","aux","definitive","procure","eisenhower","almighty","apples","ware",
"haste","nail","museums","concession","utterance","ingenious","scandal","grazing",
"springer","thirst","xix","rico","treasures","evangelical","microscopic","grams",
"chromosomes","reviewing","sadly","jo","ia","desperately","tourists","mann",
"potentials","emissions","seeming","agony","uptake","conduction","greene","soviets",
"alternating","incompatible","interim","toe","striving","costa","digestion","immortality",
"raid","bitterly","eliminating","andrews","handicapped","responsive","scarlet","counselor",
"drained","supposedly","affords","learners","ribs","interpreter","inscriptions","agrarian",
"insistence","generosity","annum","nile","dye","stain","dictated","spacing",
"specificity","beads","kit","disappearance","abuses","corrosion","conductivity","warn",
"aviation","penguin","pasture","circulating","brooklyn","cafe","bin","stove",
"frances","shy","offences","col","licence","sacrificed","prentice","sandstone",
"starch","madras","rue","cooled","stout","simpson","yeast","athletic",
"choir","faithfully","barren","manpower","brutal","lets","edgar","skins",
"ken","baldwin","pots","expedient","confront","comprises","inferred","corrections",
"duck","loudly","couch","halls","fin","garments","ted","demonstrating",
"terry","seminary","biographical","assent","cp","solitude","erroneous","calvin",
"kingdoms","establishes","throws","keith","insult","mixtures","prudence","bitterness",
"balancing","coke","humility","lobe","treats","provoked","supervisors","patents",
"cox","folds","eden","suggestive","bowel","counterpart","nietzsche","viral",
"robust","eloquence","purse","mosaic","ottoman","volatile","henderson","intimately",
"protocols","willingly","deficiencies","astronomy","cleaned","toll","hydraulic","twist",
"openings","accountability","chooses","indicative","betty","melted","pub","aus",
"paramount","finer","delegate","damned","rob","blocking","psalm","exercising",
"coincidence","multiplication","dallas","flavor","mythology","credibility","pillars","markers",
"richer","enrolled","preventive","abused","influencing","ardent","blend","denver",
"adjusting","lily","witch","workshops","impatient","arthritis","wellington","nowadays",
"authoritative","ceremonial","pouring","maiden","mantle","tracing","rationality","refrain",
"insofar","germ","benign","expelled","toys","debris","arches","broadway",
"wartime","factual","signifies","notorious","brook","suffers","parallels","protests",
"keeper","awe","telegram","lf","terminate","advertisement","maize","chopped",
"mon","strife","buck","neighbour","afghanistan","barley","coated","licensing",
"accorded","examines","amateur","metres","learns","veteran","divorced","davies",
"dot","lid","maxwell","supplemented","db","lifestyle","overthrow","missile",
"wordsworth","investor","predominant","apex","drought","emigration","collapsed","signing",
"peking","yang","pact","discouraged","fruitful","documentary","claude","ballot",
"distribute","embarrassment","miniature","portrayed","genetics","sounding","peculiarly","piston",
"blamed","apartments","lebanon","statues","prominence","radioactive","disregard","grim",
"intensely","countess","wastes","premise","socialists","compel","pointer","comprise",
"bile","implements","nutrient","careless","grammatical","punjab","turbulent","victorious",
"arid","uneasy","worm","radar","hearted","negotiating","burma","gerald",
"sands","hiring","ornaments","licensed","woven","practicable","combines","amplifier",
"reductions","worms","shine","palestinian","culturally","morale","dialect","penal",
"leslie","stamped","retarded","verification","pierce","hen","backing","blues",
"discarded","han","chapman","delicious","prognosis","coats","contractors","isaiah",
"brazilian","isle","injunction","nonlinear","replies","disclosed","lbs","telescope",
"privy","patrol","rotating","hepatic","hints","irving","hymn","slain",
"mouths","exalted","registers","charleston","frontiers","funded","tolerate","sperm",
"dispose","steamer","abandonment","ventral","attorneys","parted","propriety","disagree",
"brightness","dewey","suburban","newer","insane","superstition","norwegian","recollection",
"intra","hears","miami","reactive","goat","nominated","chen","leipzig",
"dip","friedrich","overwhelmed","testify","flank","submarine","feminism","swallow",
"dennis","magnet","canoe","banner","canterbury","pools","copying","fa",
"stationed","wa","tossed","toy","astonishing","reich","weeds","sensibility",
"timothy","proposes","morton","traumatic","coral","deficits","lesbian","fi",
"allowances","descend","sequential","heap","insists","earnestly","ga","passim",
"warner","asylum","purification","thereto","penetrating","opium","naive","hats",
"minneapolis","ethnicity","poll","nt","inst","affectionate","faction","nut",
"liquids","questionable","jupiter","clue","solo","partisan","integrate","migrants",
"henri","mucous","aristocratic","oblique","crane","pragmatic","broker","howe",
"economist","assay","shout","drilling","flowering","disputed","ineffective","pollen",
"confederation","climax","ryan","industrialization","bliss","aaron","statistically","pneumonia",
"kick","honestly","cl","consecutive","supplier","norfolk","skeletal","collectively",
"abbot","impress","hare","hispanic","cough","methyl","whisper","clara",
"finances","essex","construed","curiously","sadness","rhode","cooperate","swiftly",
"familiarity","grinding","sinus","durham","garment","mold","stockholders","atlas",
"hello","pistol","mischief","rounds","financed","timely","malcolm","connexion",
"recipients","enquiry","dismissal","progressively","surveyed","wiped","terminals","decidedly",
"vermont","warehouse","unprecedented","gloomy","generalization","jerome","batch","novelist",
"symbolism","pyramid","gigantic","messiah","chips","exotic","realise","burton",
"warsaw","cultivate","spontaneously","certification","cortical","polarization","bombs","everlasting",
"nevada","amused","clement","omission","flowed","proprietor","diana","pseudo",
"containers","sulfur","prophetic","designers","rains","repaired","ups","astonishment",
"gazed","procedural","hague","seals","dominions","purified","armstrong","loosely",
"henceforth","sulphate","awakening","intervening","lib","unrelated","dumb","ian",
"violently","hindus","seminar","resin","telecommunications","priori","credited","encyclopedia",
"evaluations","eventual","calories","modernity","vivo","reside","boot","brotherhood",
"genera","massacre","predecessor","smiles","dwellings","unsatisfactory","cylindrical","manor",
"rushing","medications","bushes","dec","digging","offender","stevenson","weaving",
"discontent","affective","noteworthy","periodically","ironically","murmured","fills","serpent",
"prototype","chester","dei","tertiary","centralized","manila","inscribed","fulfil",
"socioeconomic","institutes","delivering","incision","aerial","undergone","bourgeoisie","disgust",
"conrad","nova","deductions","irrational","epithelium","barnes","orbital","var",
"critically","ag","homosexual","ambitions","hairs","backwards","rubbed","heal",
"hannah","salad","sectional","dial","hemorrhage","methodological","victories","foe",
"rivals","activists","vendor","sensor","ounces","lenses","adherence","supp",
"liberated","shillings","tending","stat","differentiate","pumps","poultry","implicitly",
"classics","disgrace","vs","disturb","indefinite","infer","harmonic","montana",
"reconcile","neatly","modem","anesthesia","verified","michel","marxism","baseline",
"necessities","stark","republics","endured","pilots","metric","apollo","builders",
"tray","schematic","leagues","employs","toilet","generates","puzzled","gallant",
"refinement","analysts","airplane","novelty","denounced","weed","ventures","internally",
"postal","portal","undergoing","awaiting","gloves","lobby","consume","admiralty",
"explosive","humidity","materially","distinguishes","wandered","decimal","clarendon","enriched",
"databases","nathan","oscar","distrust","inorganic","invoked","palms","linguistics",
"ventured","alliances","unfamiliar","gown","belgian","procured","bei","splitting",
"ante","shattered","ceases","dysfunction","expended","deferred","affirm","trench",
"mound","slid","craig","candles","aspiration","transitional","mobilization","charcoal",
};
// clang-format on

} // namespace detail
} // namespace en
} // namespace fea
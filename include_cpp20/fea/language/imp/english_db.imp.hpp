// English datasets, other than the dictionary.

namespace fea {
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

} // namespace detail
} // namespace en
} // namespace fea
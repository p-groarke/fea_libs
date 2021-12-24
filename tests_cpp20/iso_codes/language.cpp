#include <fea/iso_codes/language.hpp>
#include <gtest/gtest.h>

namespace {
TEST(language, basics) {
	for (size_t i = 0; i < fea::detail::iso_639_languages.size(); ++i) {
		const fea::iso_639_lang& l = fea::detail::iso_639_languages[i];
		EXPECT_EQ(fea::iso_639_id_t(i), l.runtime_id);

		if (l.parent_macro_id != (std::numeric_limits<uint16_t>::max)()) {
			EXPECT_FALSE(l.parent_macro_code_3.empty());
		}

		//{
		//	const fea::iso_639_lang& l3 = fea::iso_639_3_lookup(l.code_3);
		//	EXPECT_EQ(l.name, l3.name);
		//	EXPECT_EQ(l.runtime_id, l3.runtime_id);
		//}
		// if (!l.code_2b.empty()) {
		//	const fea::iso_639_lang& l2b = fea::iso_639_2b_lookup(l.code_2b);
		//	EXPECT_EQ(l.name, l2b.name);
		//	EXPECT_EQ(l.runtime_id, l2b.runtime_id);
		//}
		// if (!l.code_2t.empty()) {
		//	const fea::iso_639_lang& l2t = fea::iso_639_2t_lookup(l.code_2t);
		//	EXPECT_EQ(l.name, l2t.name);
		//	EXPECT_EQ(l.runtime_id, l2t.runtime_id);
		//}
		// if (!l.code_1.empty()) {
		//	const fea::iso_639_lang& l1 = fea::iso_639_1_lookup(l.code_1);
		//	EXPECT_EQ(l.name, l1.name);
		//	EXPECT_EQ(l.runtime_id, l1.runtime_id);
		//}
	}
}
} // namespace

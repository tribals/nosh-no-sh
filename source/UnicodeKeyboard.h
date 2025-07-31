/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_UNICODEKEYBOARD_H)
#define INCLUDE_UNICODEKEYBOARD_H

/// \brief Utilities for implementing combining keys per ISO 9995-3 and Unicode composition rules.
namespace UnicodeKeyboard {

/// \brief Combine multiple dead keys into different dead keys.
bool
combine_dead_keys (
	char32_t & c1,
	const char32_t c2
) ;

bool
combine_peculiar_non_combiners (
	const char32_t c,
	char32_t & b
) ;

bool
combine_grotty_combiners (
	const char32_t c,
	char32_t & b
) ;

bool
combine_unicode (
	const char32_t c,
	char32_t & b
) ;

bool
lower_combining_class (
	char32_t c1,
	char32_t c2
) ;

}

#endif

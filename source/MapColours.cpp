/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <stdint.h>
#include "CharacterCell.h"

const CharacterCell::colour_type ColourPair::colour_type::erased_foreground(ALPHA_FOR_ERASED,0xC0,0xC0,0xC0);
const CharacterCell::colour_type ColourPair::colour_type::erased_background(ALPHA_FOR_ERASED,0U,0U,0U);
namespace {
const CharacterCell::colour_type dim_erased_foreground(ALPHA_FOR_ERASED,0x80,0x80,0x80);
const CharacterCell::colour_type bright_erased_background(ALPHA_FOR_ERASED,0x40,0x40,0x40);
}
const CharacterCell::colour_type ColourPair::colour_type::default_foreground(ALPHA_FOR_DEFAULT,0xC0,0xC0,0xC0);
const CharacterCell::colour_type ColourPair::colour_type::default_background(ALPHA_FOR_DEFAULT,0U,0U,0U);
const CharacterCell::colour_type ColourPair::colour_type::impossible(-1U,0,0,0);
const ColourPair ColourPair::impossible(ColourPair::colour_type::impossible, ColourPair::colour_type::impossible);
const ColourPair ColourPair::def(ColourPair::colour_type::default_foreground, ColourPair::colour_type::default_background);
const ColourPair ColourPair::erased(dim_erased_foreground, bright_erased_background);
const ColourPair ColourPair::white_on_black(Map256Colour(COLOUR_WHITE), Map256Colour(COLOUR_BLACK));

CharacterCell::colour_type
Map16Colour (
	uint8_t c
) {
	c %= 16U;
	if (7U == c) {
		// Dark white is brighter than bright black.
		return CharacterCell::colour_type(ALPHA_FOR_16_COLOURED,0xBF,0xBF,0xBF);
	} else if (4U == c) {
		// Everyone fusses about dark blue, and no choice is perfect.
		// This choice is Web Indigo.
		return CharacterCell::colour_type(ALPHA_FOR_16_COLOURED,0x4B,0x00,0x82);
	} else {
		if (8U == c) c = 7U;	// Substitute original dark white for bright black, which otherwise would work out the same as dark black.
		const uint8_t h((c & 8U)? 255U : 127U), b(c & 4U), g(c & 2U), r(c & 1U);
		return CharacterCell::colour_type(ALPHA_FOR_16_COLOURED,r ? h : 0U,g ? h : 0U,b ? h : 0U);
	}
}

CharacterCell::colour_type
Map256Colour (
	uint8_t c
) {
	if (c < 16U) {
		CharacterCell::colour_type r(Map16Colour(c));
		r.alpha = ALPHA_FOR_256_COLOURED;
		return r;
	} else if (c < 232U) {
		c -= 16U;
		uint8_t b(c % 6U), g((c / 6U) % 6U), r(c / 36U);
		if (r > 0U) r = r * 40U + 55U;
		if (g > 0U) g = g * 40U + 55U;
		if (b > 0U) b = b * 40U + 55U;
		return CharacterCell::colour_type(ALPHA_FOR_256_COLOURED,r,g,b);
	} else {
		c -= 232U;
		return CharacterCell::colour_type(ALPHA_FOR_256_COLOURED,c * 10U + 8U,c * 10U + 8U,c * 10U + 8U);
	}
}

CharacterCell::colour_type
MapTrueColour (
	uint8_t r,
	uint8_t g,
	uint8_t b
) {
	return CharacterCell::colour_type(ALPHA_FOR_TRUE_COLOURED,r,g,b);
}

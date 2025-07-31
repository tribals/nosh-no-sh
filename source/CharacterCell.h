/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_CHARACTERCELL_H)
#define INCLUDE_CHARACTERCELL_H

#include <stdint.h>

// ECMA-48 standard indexed colours.
enum { COLOUR_BLACK, COLOUR_RED, COLOUR_GREEN, COLOUR_YELLOW, COLOUR_BLUE, COLOUR_MAGENTA, COLOUR_CYAN, COLOUR_WHITE };
// Conventional but non-standard indexed colours.
enum { COLOUR_DARK_VIOLET = 92, COLOUR_DARK_ORANGE1 = 64, COLOUR_DARK_ORANGE3 = 130, COLOUR_LIGHT_ORANGE = 0xD6, COLOUR_LIGHT_CYAN = 50 };

enum {
	ALPHA_FOR_ERASED	= 0U,
	ALPHA_FOR_DEFAULT	= 1U,
	ALPHA_FOR_16_COLOURED	= 2U,
	ALPHA_FOR_256_COLOURED	= 3U,
	ALPHA_FOR_TRUE_COLOURED	= 4U,
	ALPHA_FOR_MOUSE_SPRITE	= 31U,
};

struct CursorSprite {
	typedef uint8_t attribute_type;
	enum {
		VISIBLE = 1U << 0U,
		BLINK = 1U << 1U,
	};
	enum glyph_type {
		UNDERLINE = 0U,
		BAR = 1U,
		BOX = 2U,
		BLOCK = 3U,
		STAR = 4U,
		UNDEROVER = 5U,
		MIRRORL = 6U
	};
};
struct PointerSprite {
	typedef uint8_t attribute_type;
	enum {
		VISIBLE = 1U << 0U,
	};
};
struct ScreenFlags {
	typedef uint8_t flag_type;
	enum {
		INVERTED = 1U << 0U
	};
};
struct ColourPair {
	struct colour_type {
		colour_type(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : alpha(a), red(r), green(g), blue(b) {}
		colour_type() : alpha(), red(), green(), blue() {}
		uint8_t alpha, red, green, blue;
		static uint_fast8_t dim (uint8_t c) { return c > 0x40 ? c - 0x40 : 0x00; }
		void dim () { red = dim(red); green = dim(green); blue = dim(blue); }
		static uint_fast8_t bright (uint8_t c) { return c < 0xC0 ? c + 0x40 : 0xff; }
		void bright () { red = bright(red); green = bright(green); blue = bright(blue); }
		bool is_default_or_erased() const { return ALPHA_FOR_ERASED == alpha || ALPHA_FOR_DEFAULT == alpha; }
		bool is_black() const { return 0 == red && 0 == green && 0 == blue; }
		static uint_fast8_t complement(uint_fast8_t c) { return ~c; }
		void complement() { red = complement(red); green = complement(green); blue = complement(blue); }
		static const colour_type erased_foreground, erased_background;
		static const colour_type default_foreground, default_background;
		static const colour_type impossible;
	};
	colour_type foreground, background;

	ColourPair(colour_type f, colour_type b) : foreground(f), background(b) {}
	ColourPair() : foreground(), background() {}

	static const ColourPair impossible, def, erased, white_on_black;
};
struct ColourPairAndAttributes : public ColourPair {
	typedef uint16_t attribute_type;
	attribute_type attributes;

	enum {
		BOLD = 1U << 0U,
		ITALIC = 1U << 1U,
		OVERLINE = 1U << 2U,
		BLINK = 1U << 3U,
		INVERSE = 1U << 4U,
		STRIKETHROUGH = 1U << 5U,
		INVISIBLE = 1U << 6U,
		FAINT = 1U << 7U,
		UNDERLINES = 15U << 8U,
			SIMPLE_UNDERLINE = 1U << 8U,
			DOUBLE_UNDERLINE = 2U << 8U,
			CURLY_UNDERLINE = 3U << 8U,
			DOTTED_UNDERLINE = 4U << 8U,
			DASHED_UNDERLINE = 5U << 8U,
			LDOTTED_UNDERLINE = 6U << 8U,
			LDASHED_UNDERLINE = 7U << 8U,
			LCURLY_UNDERLINE = 8U << 8U,
			LLDOTTED_UNDERLINE = 9U << 8U,
			LLDASHED_UNDERLINE = 10U << 8U,
		FRAME = 1U << 12U,
		ENCIRCLE = 1U << 13U,
	};

	ColourPairAndAttributes(attribute_type a, colour_type f, colour_type b) : ColourPair(f, b), attributes(a) {}
	ColourPairAndAttributes(attribute_type a, const ColourPair & p) : ColourPair(p), attributes(a) {}
	ColourPairAndAttributes() : ColourPair(), attributes() {}
};
struct CharacterCell : ColourPairAndAttributes {
	typedef char32_t character_type;

	CharacterCell(character_type c, attribute_type a, colour_type f, colour_type b) : ColourPairAndAttributes(a, f, b), character(c) {}
	CharacterCell(character_type c, attribute_type a, const ColourPair & p) : ColourPairAndAttributes(a, p), character(c) {}
	CharacterCell(character_type c, const ColourPairAndAttributes & p) : ColourPairAndAttributes(p), character(c) {}
	CharacterCell() : ColourPairAndAttributes(), character() {}

	character_type character;
};

inline
bool
operator != (
	const CharacterCell::colour_type & a,
	const CharacterCell::colour_type & b
) {
	return a.alpha != b.alpha || a.red != b.red || a.green != b.green || a.blue != b.blue;
}

extern
CharacterCell::colour_type
Map16Colour (
	uint8_t c
) ;
extern
CharacterCell::colour_type
Map256Colour (
	uint8_t c
) ;
extern
CharacterCell::colour_type
MapTrueColour (
	uint8_t r,
	uint8_t g,
	uint8_t b
) ;

#endif

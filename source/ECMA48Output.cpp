/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include "ECMA48Output.h"
#include "TerminalCapabilities.h"

namespace {

inline
uint_fast16_t
SquareDiff(
	uint_fast8_t v1,
	uint_fast8_t v2
) {
	uint_fast8_t v(v1 > v2 ? v1 - v2 : v2 - v1);
	return uint_fast16_t(v) * v;
}

inline
uint_fast32_t
PythagoreanDistance (
	const CharacterCell::colour_type & a,
	const CharacterCell::colour_type & b
) {
	return uint_fast32_t(SquareDiff(a.alpha, b.alpha)) + SquareDiff(a.red, b.red) + SquareDiff(a.green, b.green) + SquareDiff(a.blue, b.blue);
}

}

void
ECMA48Output::print_control_character(
	unsigned char character
) const {
        if (c1_7bit) {
                if (character >= 0x80) {
                        std::putc(ESC, out);
                        character -= 0x40;
                }
                std::putc(character, out);
        } else
        if (c1_8bit)
                std::putc(character, out);
        else
                UTF8(character);
}

void
ECMA48Output::print_control_characters(
	unsigned char character,
	unsigned int n
) const {
	while (n) {
		--n;
		print_control_character(character);
	}
}

void
ECMA48Output::newline() const
{
	if (!caps.lacks_NEL)
		print_control_character(NEL);
	else
	{
		print_control_character(CR);
		print_control_character(LF);
	}
}

void
ECMA48Output::reverse_index() const
{
	print_control_character(RI);
}

void
ECMA48Output::forward_index() const
{
	print_control_character(IND);
}

void
ECMA48Output::UTF8(
	uint32_t ch
) const {
	if (ch < 0x00000080) {
		const char s[1] = {
			static_cast<char>(ch)
		};
		std::fwrite(s, sizeof s, 1, out);
	} else
	if (ch < 0x00000800) {
		const char s[2] = {
			static_cast<char>(0xC0 | (0x1F & (ch >> 6U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 0U))),
		};
		std::fwrite(s, sizeof s, 1, out);
	} else
	if (ch < 0x00010000) {
		const char s[3] = {
			static_cast<char>(0xE0 | (0x0F & (ch >> 12U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 6U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 0U))),
		};
		std::fwrite(s, sizeof s, 1, out);
	} else
	if (ch < 0x00200000) {
		const char s[4] = {
			static_cast<char>(0xF0 | (0x07 & (ch >> 18U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 12U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 6U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 0U))),
		};
		std::fwrite(s, sizeof s, 1, out);
	} else
	if (ch < 0x04000000) {
		const char s[5] = {
			static_cast<char>(0xF8 | (0x03 & (ch >> 24U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 18U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 12U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 6U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 0U))),
		};
		std::fwrite(s, sizeof s, 1, out);
	} else
	{
		const char s[6] = {
			static_cast<char>(0xFC | (0x01 & (ch >> 30U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 24U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 18U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 12U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 6U))),
			static_cast<char>(0x80 | (0x3F & (ch >> 0U))),
		};
		std::fwrite(s, sizeof s, 1, out);
	}
}

void
ECMA48Output::change_cursor_visibility(
	bool v
) const {
	if (caps.use_DECPrivateMode)
		DECTCEM(v);
}

void
ECMA48Output::SCUSR(
	CursorSprite::attribute_type a,
	CursorSprite::glyph_type g
) const {
	switch (caps.cursor_shape_command) {
		case TerminalCapabilities::NO_SCUSR:
			if (caps.use_DECPrivateMode)
				DECPrivateMode(12U, a & CursorSprite::BLINK);
			break;
		case TerminalCapabilities::ORIGINAL_DECSCUSR:
			switch (g) {
				case CursorSprite::BAR:		[[clang::fallthrough]];
				case CursorSprite::UNDEROVER:	[[clang::fallthrough]];
				case CursorSprite::UNDERLINE:	DECSCUSR(CursorSprite::BLINK & a ? 3U : 4U); break;
				case CursorSprite::MIRRORL:	[[clang::fallthrough]];
				case CursorSprite::BOX:		[[clang::fallthrough]];
				case CursorSprite::STAR:	[[clang::fallthrough]];
				case CursorSprite::BLOCK:	DECSCUSR(CursorSprite::BLINK & a ? 1U : 2U); break;
#if 0	// Actually unreachable, and generates a warning.
				default:			DECSCUSR(0U); break;
#endif
			}
			break;
		case TerminalCapabilities::XTERM_DECSCUSR:
			switch (g) {
				case CursorSprite::BAR:		DECSCUSR(CursorSprite::BLINK & a ? 5U : 6U); break;
				case CursorSprite::UNDEROVER:	[[clang::fallthrough]];
				case CursorSprite::UNDERLINE:	DECSCUSR(CursorSprite::BLINK & a ? 3U : 4U); break;
				case CursorSprite::MIRRORL:	[[clang::fallthrough]];
				case CursorSprite::BOX:		[[clang::fallthrough]];
				case CursorSprite::STAR:	[[clang::fallthrough]];
				case CursorSprite::BLOCK:	DECSCUSR(CursorSprite::BLINK & a ? 1U : 2U); break;
#if 0	// Actually unreachable, and generates a warning.
				default:			DECSCUSR(0U); break;
#endif
			}
			break;
		case TerminalCapabilities::EXTENDED_DECSCUSR:
			switch (g) {
				case CursorSprite::BLOCK:	DECSCUSR(CursorSprite::BLINK & a ? 1U : 2U); break;
				case CursorSprite::UNDERLINE:	DECSCUSR(CursorSprite::BLINK & a ? 3U : 4U); break;
				case CursorSprite::BAR:		DECSCUSR(CursorSprite::BLINK & a ? 5U : 6U); break;
				case CursorSprite::BOX:		DECSCUSR(CursorSprite::BLINK & a ? 7U : 8U); break;
				case CursorSprite::STAR:	DECSCUSR(CursorSprite::BLINK & a ? 9U : 10U); break;
				case CursorSprite::UNDEROVER:	DECSCUSR(CursorSprite::BLINK & a ? 11U : 12U); break;
				case CursorSprite::MIRRORL:	DECSCUSR(CursorSprite::BLINK & a ? 13U : 14U); break;
#if 0	// Actually unreachable, and generates a warning.
				default:			DECSCUSR(0U); break;
#endif
			}
			break;
		case TerminalCapabilities::LINUX_SCUSR:
			switch (g) {
				case CursorSprite::UNDEROVER:	[[clang::fallthrough]];
				case CursorSprite::UNDERLINE:	[[clang::fallthrough]];
				case CursorSprite::BAR:		LINUXSCUSR(1U); break;
				case CursorSprite::MIRRORL:	[[clang::fallthrough]];
				case CursorSprite::BOX:		[[clang::fallthrough]];
				case CursorSprite::STAR:	[[clang::fallthrough]];
				case CursorSprite::BLOCK:	LINUXSCUSR(8U); break;
#if 0	// Actually unreachable, and generates a warning.
				default:			LINUXSCUSR(4U); break;
#endif
			}
			break;
	}
}

void
ECMA48Output::SCUSR() const
{
	switch (caps.cursor_shape_command) {
		case TerminalCapabilities::NO_SCUSR:
			break;
		case TerminalCapabilities::ORIGINAL_DECSCUSR:
		case TerminalCapabilities::XTERM_DECSCUSR:
		case TerminalCapabilities::EXTENDED_DECSCUSR:
			DECSCUSR();
			break;
		case TerminalCapabilities::LINUX_SCUSR:
			LINUXSCUSR();
			break;
	}
}

void
ECMA48Output::SGRColour(
	bool is_fg
) const {
	if (TerminalCapabilities::NO_COLOURS == caps.colour_level) return;
	csi();
	std::fprintf(out, "%um", is_fg ? 39U : 49U);
}

void
ECMA48Output::SGRColour(
	bool is_fg,
	const CharacterCell::colour_type & colour
) const {
	if (TerminalCapabilities::NO_COLOURS == caps.colour_level) return;
	if (colour.is_default_or_erased()) {
		SGRColour(is_fg);
		return;
	}
	// If we know that the RGB triple came from an ECMA-48 standard colour or AIXTerm colour in the first place ...
	if (ALPHA_FOR_16_COLOURED == colour.alpha) {
		// ... if we would normally use indexed or direct colour, see whether we can use an exact match ECMA-48 shorter sequence.
		switch (caps.colour_level) {
			case TerminalCapabilities::INDEXED_COLOUR_FAULTY:
			case TerminalCapabilities::ISO_INDEXED_COLOUR:
			case TerminalCapabilities::DIRECT_COLOUR_FAULTY:
			case TerminalCapabilities::ISO_DIRECT_COLOUR:
				// Only test for the standard ECMA-48 colours.
				// Colours above 7 might erroneously end up as blinking or bold or something.
				for (uint_least8_t i(0U); i < 8U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map16Colour(i), colour));
					if (0 == d) {
						SGRColour16(is_fg, i);
						return;
					}
				}
				break;
			case TerminalCapabilities::NO_COLOURS:
			case TerminalCapabilities::ECMA_8_COLOURS:
			case TerminalCapabilities::ECMA_16_COLOURS:
				break;
		}
	}
	// If we know that the RGB triple came from an ECMA-48 standard colour, AIXTerm colour, or an indexed colour in the first place ...
	if (ALPHA_FOR_16_COLOURED == colour.alpha || ALPHA_FOR_256_COLOURED == colour.alpha) {
		// ... if we would normally use direct colour, see whether we can use an exact match indexed colour shorter sequence.
		switch (caps.colour_level) {
			case TerminalCapabilities::DIRECT_COLOUR_FAULTY:
			{
				uint_least16_t closest(-1U);
				for (uint_least16_t i(0U); i < 256U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map256Colour(i), colour));
					if (0 == d) closest = i;
				}
				if (closest < 256U) {
					SGRColour256Ambig(is_fg, closest);
					return;
				}
				break;
			}
			case TerminalCapabilities::ISO_DIRECT_COLOUR:
			{
				uint_least16_t closest(-1U);
				for (uint_least16_t i(0U); i < 256U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map256Colour(i), colour));
					if (0 == d) closest = i;
				}
				if (closest < 256U) {
					SGRColour256(is_fg, closest);
					return;
				}
				break;
			}
			case TerminalCapabilities::NO_COLOURS:
			case TerminalCapabilities::ECMA_8_COLOURS:
			case TerminalCapabilities::ECMA_16_COLOURS:
			case TerminalCapabilities::INDEXED_COLOUR_FAULTY:
			case TerminalCapabilities::ISO_INDEXED_COLOUR:
				break;
		}
	}
	// No optimizations left; just use the closest-to-truecolour mechanism that the terminal is capabile of.
	switch (caps.colour_level) {
		case TerminalCapabilities::NO_COLOURS:
			return;
		case TerminalCapabilities::ECMA_8_COLOURS:
			{
				uint_fast32_t dist(-1U);
				uint_least8_t closest(0U);
				for (uint_least8_t i(0U); i < 8U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map16Colour(i), colour));
					if (d < dist) {
						closest = i;
						dist = d;
						if (0 == dist) break;
					}
				}
				SGRColour8(is_fg, closest);
			}
			return;
		case TerminalCapabilities::ECMA_16_COLOURS:
			{
				uint_fast32_t dist(-1U);
				uint_least8_t closest(0U);
				for (uint_least8_t i(0U); i < 16U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map16Colour(i), colour));
					if (d < dist) {
						closest = i;
						dist = d;
						if (0 == dist) break;
					}
				}
				SGRColour16(is_fg, closest);
			}
			return;
		case TerminalCapabilities::INDEXED_COLOUR_FAULTY:
			{
				uint_fast32_t dist(-1U);
				uint_least16_t closest(0U);
				// We prefer the non-AIXTerm colours normally, as the AIXTerm colours are often remapped by "colourschemes".
				// However, when the colours came from AIXTerm colours in the first place, we prefer them.
				const bool prefer_standard(ALPHA_FOR_16_COLOURED == colour.alpha);
				for (uint_least16_t i(0U); i < 256U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map256Colour(i), colour));
					if (prefer_standard ? d < dist : d <= dist) {
						closest = i;
						dist = d;
						if (prefer_standard && 0 == dist) break;
					}
				}
				SGRColour256Ambig(is_fg, closest);
			}
			return;
		case TerminalCapabilities::ISO_INDEXED_COLOUR:
			{
				uint_fast32_t dist(-1U);
				uint_least16_t closest(0U);
				// See afore.
				const bool prefer_standard(ALPHA_FOR_16_COLOURED == colour.alpha);
				for (uint_least16_t i(0U); i < 256U; ++i) {
					const uint_fast32_t d(PythagoreanDistance(Map256Colour(i), colour));
					if (prefer_standard ? d < dist : d <= dist) {
						closest = i;
						dist = d;
						if (prefer_standard && 0 == dist) break;
					}
				}
				SGRColour256(is_fg, closest);
			}
			return;
		case TerminalCapabilities::DIRECT_COLOUR_FAULTY:
			SGRTrueColourAmbig(is_fg, colour.red, colour.green, colour.blue);
			return;
		case TerminalCapabilities::ISO_DIRECT_COLOUR:
			SGRTrueColour(is_fg, colour.red, colour.green, colour.blue);
			return;
	}
}

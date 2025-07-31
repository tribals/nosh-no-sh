/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstddef>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "CharacterCell.h"
#include "TUIDisplayCompositor.h"
#include "TUIVIO.h"
#include "UTF8Decoder.h"
#include "ControlCharacters.h"

/* UTF-8 Decoder helper class ***********************************************
// **************************************************************************
*/

namespace {

struct UTF8DecoderHelper :
	public UTF8Decoder,
	public UTF8Decoder::UCS32CharacterSink
{
	UTF8DecoderHelper(TUIVIO &, long, long &, const CharacterCell::attribute_type &, const ColourPair &);
protected:
	TUIVIO & vio;
	long row;
	long & col;
	const CharacterCell::attribute_type & attr;
	const ColourPair & colour;

	virtual void ProcessDecodedUTF8(char32_t , bool, bool);
};

}

UTF8DecoderHelper::UTF8DecoderHelper(
	TUIVIO & v,
	long r,
	long & c,
	const CharacterCell::attribute_type & a,
	const ColourPair & p
) :
	UTF8Decoder(*static_cast<UTF8Decoder::UCS32CharacterSink *>(this)),
	vio(v),
	row(r),
	col(c),
	attr(a),
	colour(p)
{
}

void
UTF8DecoderHelper::ProcessDecodedUTF8(
	char32_t character,
	bool decoder_error,
	bool /*overlong*/
) {
	CharacterCell::attribute_type a(attr);
	if (decoder_error)
		a ^= CharacterCell::INVERSE;
	vio.PrintNCharsAttr(row, col, attr, colour, character, 1U);
}

/* The TUIVIO class *********************************************************
// **************************************************************************
*/

TUIVIO::TUIVIO(
        TUIDisplayCompositor & comp
) :
        c(comp)
{
}

inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	CharacterCell::character_type character,
	std::size_t l
) {
	CharacterCell cell(character, attr, colour);
	while (l) {
		--l;
		c.poke(row, col++, cell);
	}
}

inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	CharacterCell cell('\0', attr, colour);
	while (l) {
		--l;
		cell.character = *b++;
		c.poke(row, col++, cell);
	}
}

inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	std::size_t l
) {
	CharacterCell cell('\0', attr, colour);
	while (l) {
		--l;
		cell.character = c.new_at(row, col).character;
		c.poke(row, col++, cell);
	}
}

#if 0	// Unused for now.
inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	while (l) {
		--l;
		CharacterCell cell(c.new_at(row, col));
		cell.character = *b++;
		c.poke(row, col, cell);
		++col;
	}
}
#endif

inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * b,
	std::size_t l
) {
	CharacterCell cell('\0', attr, colour);
	while (l) {
		--l;
		cell.character = *b++;
		c.poke(row, col++, cell);
	}
}

#if 0	// Unused for now.
inline
void
TUIVIO::poke_quick (
	long row,
	long col,
	const char * b,
	std::size_t l
) {
	while (l) {
		--l;
		CharacterCell cell(c.new_at(row, col));
		cell.character = *b++;
		c.poke(row, col, cell);
		++col;
	}
}
#endif

void
TUIVIO::PrintNCharsAttr (
	long row,
	long & col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	CharacterCell::character_type character,
	unsigned n
) {
	if (col < 0) {
		if (-col >= n) {
			col += n;
			return;
		}
		n -= -col;
		col = 0;
	}
	if (0 <= row && row < c.query_h() && col < c.query_w()) {
		const unsigned l(col + n > c.query_w() ? c.query_w() - col : n);
		poke_quick(row, col, attr, colour, character, l);
	}
	col += n;
}

void
TUIVIO::PrintNAttrs (
        long row,
        long & col,
        const CharacterCell::attribute_type & attr,
        const ColourPair & colour,
        unsigned n
) {
	if (col < 0) {
		if (-col >= n) {
			col += n;
			return;
		}
		n -= -col;
		col = 0;
	}
	if (0 <= row && row < c.query_h() && col < c.query_w()) {
		const unsigned l(col + n > c.query_w() ? c.query_w() - col : n);
		poke_quick(row, col, attr, colour, l);
	}
	col += n;
}

void
TUIVIO::PrintCharStrAttr (
	long row,
	long & col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	if (col < 0) {
		if (static_cast<std::size_t>(-col) >= l) {
			col += l;
			return;
		}
		b += -col;
		l -= -col;
		col = 0;
	}
	if (0 <= row && row < c.query_h() && col < c.query_w()) {
		const std::size_t n(col + l > c.query_w() ? c.query_w() - col : l);
		poke_quick(row, col, attr, colour, b, n);
	}
	col += l;
}

void
TUIVIO::PrintCharStrAttr7Bit (
	long row,
	long & col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * b,
	std::size_t l
) {
	if (col < 0) {
		if (static_cast<std::size_t>(-col) >= l) {
			col += l;
			return;
		}
		b += -col;
		l -= -col;
		col = 0;
	}
	if (0 <= row && row < c.query_h() && col < c.query_w()) {
		const std::size_t n(col + l > c.query_w() ? c.query_w() - col : l);
		poke_quick(row, col, attr, colour, b, n);
	}
	col += l;
}

void
TUIVIO::PrintCharStrAttrUTF8 (
	long row,
	long & col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * b,
	std::size_t l
) {
	UTF8DecoderHelper h(*this, row, col, attr, colour);
	while (l) {
		--l;
		h.Process(*b++);
	}
}

#if 0	// Unused for now.
void
TUIVIO::PrintCharStr (
	long row,
	long & col,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	if (col < 0) {
		if (static_cast<std::size_t>(-col) >= l) {
			col += l;
			return;
		}
		l -= -col;
		col = 0;
	}
	if (0 <= row && row < c.query_h() && col < c.query_w()) {
		const std::size_t n(col + l > c.query_w() ? c.query_w() - col : l);
		poke_quick(row, col, b, n);
	}
	col += l;
}
#endif

void
TUIVIO::PrintFormatted7Bit (
	long row,
	long & col,
	std::size_t max,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * format,
	...
) {
	char *buf(nullptr);
	va_list a;
	va_start(a, format);
	const int n(vasprintf(&buf, format, a));
	va_end(a);
	if (buf && 0 <= n)
		PrintCharStrAttr7Bit(row, col, attr, colour, buf, static_cast<std::size_t>(n) < max ? n : max);
	free(buf);
}

void
TUIVIO::PrintFormattedUTF8 (
	long row,
	long & col,
	std::size_t max,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * format,
	...
) {
	char *buf(nullptr);
	va_list a;
	va_start(a, format);
	const int n(vasprintf(&buf, format, a));
	va_end(a);
	if (buf && 0 <= n)
		PrintCharStrAttrUTF8(row, col, attr, colour, buf, static_cast<std::size_t>(n) < max ? n : max);
	free(buf);
}

void
TUIVIO::CLS(
	const ColourPair & colour,
	CharacterCell::character_type backdrop_character
) {
	long l = c.query_w();
	for (long row = c.query_h(); row-- > 0;)
		WriteNCharsAttr (row, 0, 0, colour, backdrop_character, l);
}

void
TUIVIO::CLSToSpace(
	const ColourPair & colour
) {
	CLS(colour, SPC);
}

void
TUIVIO::CLSToHalfTone(
	const ColourPair & colour
) {
	CLS(colour, 0x2592);
}

void
TUIVIO::CLSToCheckerBoardFill(
	const ColourPair & colour
) {
	CLS(colour, 0x1FB95);
}

void
TUIVIO::WriteNCharsAttr (
        long row,
        long col,
        const CharacterCell::attribute_type & attr,
        const ColourPair & colour,
        CharacterCell::character_type character,
        unsigned n
) {
	PrintNCharsAttr(row, col, attr, colour, character, n);
}

void
TUIVIO::WriteNAttrs (
        long row,
        long col,
        const CharacterCell::attribute_type & attr,
        const ColourPair & colour,
        unsigned n
) {
	PrintNAttrs(row, col, attr, colour, n);
}

void
TUIVIO::WriteCharStrAttr (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	PrintCharStrAttr(row, col, attr, colour, b, l);
}

void
TUIVIO::WriteCharStrAttr7Bit (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * b,
	std::size_t l
) {
	PrintCharStrAttr7Bit(row, col, attr, colour, b, l);
}

void
TUIVIO::WriteCharStrAttrUTF8 (
	long row,
	long col,
	const CharacterCell::attribute_type & attr,
	const ColourPair & colour,
	const char * b,
	std::size_t l
) {
	PrintCharStrAttrUTF8(row, col, attr, colour, b, l);
}

#if 0	// Unused for now.
void
TUIVIO::WriteCharStr (
	long row,
	long col,
	const CharacterCell::character_type * b,
	std::size_t l
) {
	PrintCharStr(row, col, b, l);
}
#endif

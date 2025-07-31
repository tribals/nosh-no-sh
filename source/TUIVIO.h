/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_TUIVIO_H)
#define INCLUDE_TUIVIO_H

class TUIDisplayCompositor;
#include "CharacterCell.h"

/// \brief VIO-style access to a TUIDisplayCompositor
///
/// This is modelled roughly after the VIO API of OS/2.
/// It is legal to supply negative and out of bounds coordinates; clipping is performed.
struct TUIVIO
{
	TUIVIO(TUIDisplayCompositor & comp);

	void WriteNCharsAttr (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, CharacterCell::character_type c, unsigned n);
	void WriteNAttrs (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, unsigned n);
#if 0	// Unused for now.
	void WriteCharStr (long row, long col, const CharacterCell::character_type * b, std::size_t l);
#endif
	void WriteCharStrAttr (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const CharacterCell::character_type * b, std::size_t l);
	void WriteCharStrAttr7Bit (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * b, std::size_t l);
	void WriteCharStrAttrUTF8 (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * b, std::size_t l);

	void PrintNCharsAttr (long row, long & col, const CharacterCell::attribute_type & attr, const ColourPair & colour, CharacterCell::character_type c, unsigned n);
	void PrintNAttrs (long row, long & col, const CharacterCell::attribute_type & attr, const ColourPair & colour, unsigned n);
#if 0	// Unused for now.
	void PrintCharStr (long row, long & col, const CharacterCell::character_type * b, std::size_t l);
#endif
	void PrintCharStrAttr (long row, long & col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const CharacterCell::character_type * b, std::size_t l);
	void PrintCharStrAttr7Bit (long row, long & col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * b, std::size_t l);
	void PrintCharStrAttrUTF8 (long row, long & col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * b, std::size_t l);
	void PrintFormatted7Bit (long row, long & col, std::size_t max, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * format, ...);
	void PrintFormattedUTF8 (long row, long & col, std::size_t max, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * format, ...);

	void CLSToSpace(const ColourPair & colour);
	void CLSToHalfTone(const ColourPair & colour);
	void CLSToCheckerBoardFill(const ColourPair & colour);

protected:
	TUIDisplayCompositor & c;

	void CLS(const ColourPair & colour, CharacterCell::character_type backdrop_character);
	void poke_quick (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, CharacterCell::character_type c, std::size_t l);
	void poke_quick (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, std::size_t l);
	void poke_quick (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const CharacterCell::character_type * b, std::size_t l);
	void poke_quick (long row, long col, const CharacterCell::character_type * b, std::size_t l);
	void poke_quick (long row, long col, const CharacterCell::attribute_type & attr, const ColourPair & colour, const char * b, std::size_t l);
#if 0	// Unused for now.
	void poke_quick (long row, long col, const char * b, std::size_t l);
#endif
};

#endif

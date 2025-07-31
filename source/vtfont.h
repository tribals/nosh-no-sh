/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_VTFONT_H)
#define INCLUDE_VTFONT_H

#include "packed.h"

/// The header of a vt(4) font file.
struct bsd_vtfont_header {
	uint8_t		magic[8], width, height;
	uint16_t	padding;
	uint32_t	glyphs, map_lengths[4];
} __packed;
static_assert(32 == sizeof(struct bsd_vtfont_header), "Font header structure is the wrong length.");

/// A map entry in a vt(4) font file.
struct bsd_vtfont_map_entry {
	uint32_t	character;
	uint16_t	glyph, count;
} __packed;
static_assert(8 == sizeof(struct bsd_vtfont_map_entry), "Font map entry structure is the wrong length.");

#endif

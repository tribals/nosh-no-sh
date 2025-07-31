/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstring>
#include <stdint.h>
#include <sys/mman.h>
#include "CharacterCell.h"
#include "GraphicsInterface.h"

/* Pixels *******************************************************************
// **************************************************************************
*/

namespace {

inline
uint16_t
colour15 (const CharacterCell::colour_type & colour)
{
	return (colour.alpha ? 0x8000 : 0x0000) | (uint16_t(colour.red & 0xF8) << 7U) | (uint16_t(colour.green & 0xF8) << 2U) | (uint16_t(colour.blue & 0xF8) >> 3U);
}

inline
uint16_t
colour16 (const CharacterCell::colour_type & colour)
{
	return (uint16_t(colour.red & 0xF8) << 8U) | (uint16_t(colour.green & 0xFC) << 3U) | (uint16_t(colour.blue & 0xF8) >> 3U);
}

inline
void
colour24 (uint8_t b[3], const CharacterCell::colour_type & colour)
{
	b[2] = colour.red;
	b[1] = colour.green;
	b[0] = colour.blue;
}

inline
uint32_t
colour32 (const CharacterCell::colour_type & colour)
{
	return (uint32_t(colour.red) << 16U) | (uint32_t(colour.green) << 8U) | (uint32_t(colour.blue) << 0U);
}

}

/* Bitmaps and blitting *****************************************************
// **************************************************************************
*/

GraphicsInterface::GlyphBitmap::~GlyphBitmap() {}

GraphicsInterface::ScreenBitmap::~ScreenBitmap() {}

void
GraphicsInterface::MemoryMappedScreenBitmap::Plot (unsigned short y, unsigned short x, uint16_t bits, const ColourPair & colour)
{
	if (y >= yres) return;
	void * const start(GetStartOfLine(y));
	switch (depth) {
		case 15U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t f(colour15(colour.foreground)), b(colour15(colour.background));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				p[off] = bit ? f : b;
			}
			break;
		}
		case 16U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t f(colour16(colour.foreground)), b(colour16(colour.background));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				p[off] = bit ? f : b;
			}
			break;
		}
		case 24U:
		{
			uint8_t * const p(static_cast<uint8_t *>(start) + 3U * x);
			uint8_t f[3], b[3];
			colour24(f, colour.foreground);
			colour24(b, colour.background);
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				std::memcpy(p + 3U * off, bit ? f : b, 3U);
			}
			break;
		}
		case 32U:
		{
			uint32_t * const p(static_cast<uint32_t *>(start) + x);
			const uint32_t f(colour32(colour.foreground)), b(colour32(colour.background));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				p[off] = bit ? f : b;
			}
			break;
		}
	}
}

void
GraphicsInterface::MemoryMappedScreenBitmap::Plot (unsigned short y, unsigned short x, uint16_t bits, uint16_t mask, const ColourPair colours[2])
{
	if (y >= yres) return;
	void * const start(GetStartOfLine(y));
	switch (depth) {
		case 15U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t fs[2] = { colour15(colours[0].foreground), colour15(colours[1].foreground) };
			const uint16_t bs[2] = { colour15(colours[0].background), colour15(colours[1].background) };
			for (unsigned off(16U); off-- > 0U; bits >>= 1U, mask >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				const unsigned index(mask & 1U);
				p[off] = bit ? fs[index] : bs[index];
			}
			break;
		}
		case 16U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t fs[2] = { colour16(colours[0].foreground), colour16(colours[1].foreground) };
			const uint16_t bs[2] = { colour16(colours[0].background), colour16(colours[1].background) };
			for (unsigned off(16U); off-- > 0U; bits >>= 1U, mask >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				const unsigned index(mask & 1U);
				p[off] = bit ? fs[index] : bs[index];
			}
			break;
		}
		case 24U:
		{
			uint8_t * const p(static_cast<uint8_t *>(start) + 3U * x);
			uint8_t fs[2][3], bs[2][3];
			colour24(fs[0], colours[0].foreground);
			colour24(fs[1], colours[1].background);
			colour24(bs[0], colours[0].foreground);
			colour24(bs[1], colours[1].background);
			for (unsigned off(16U); off-- > 0U; bits >>= 1U, mask >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				const unsigned index(mask & 1U);
				std::memcpy(p + 3U * off, bit ? fs[index] : bs[index], 3U);
			}
			break;
		}
		case 32U:
		{
			uint32_t * const p(static_cast<uint32_t *>(start) + x);
			const uint32_t fs[2] = { colour32(colours[0].foreground), colour32(colours[1].foreground) };
			const uint32_t bs[2] = { colour32(colours[0].background), colour32(colours[1].background) };
			for (unsigned off(16U); off-- > 0U; bits >>= 1U, mask >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				const unsigned index(mask & 1U);
				p[off] = bit ? fs[index] : bs[index];
			}
			break;
		}
	}
}

void
GraphicsInterface::MemoryMappedScreenBitmap::AlphaBlend (unsigned short y, unsigned short x, uint16_t bits, const CharacterCell::colour_type & colour)
{
	if (y >= yres) return;
	void * const start(GetStartOfLine(y));
	switch (depth) {
		case 15U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t c(colour15(colour));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				if (bit) p[off] = c;
			}
			break;
		}
		case 16U:
		{
			uint16_t * const p(static_cast<uint16_t *>(start) + x);
			const uint16_t c(colour16(colour));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				if (bit) p[off] = c;
			}
			break;
		}
		case 24U:
		{
			uint8_t * const p(static_cast<uint8_t *>(start) + 3U * x);
			uint8_t c[3];
			colour24(c, colour);
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				if (bit)
					std::memcpy(p + 3U * off, c, 3U);
			}
			break;
		}
		case 32U:
		{
			uint32_t * const p(static_cast<uint32_t *>(start) + x);
			const uint32_t c(colour32(colour));
			for (unsigned off(16U); off-- > 0U; bits >>= 1U) {
				if (x + off >= xres) continue;
				const bool bit(bits & 1U);
				if (bit) p[off] = c;
			}
			break;
		}
	}
}

void
GraphicsInterface::ApplyAttributesToGlyphBitmap(GlyphBitmapHandle handle, CharacterCell::attribute_type attributes)
{
	switch (attributes & CharacterCell::UNDERLINES) {
		case 0U:		 		break;
		case CharacterCell::SIMPLE_UNDERLINE:	handle->Plot(15U, 0xFFFF); break;
		case CharacterCell::DOUBLE_UNDERLINE:	handle->Plot(14U, 0xFFFF); handle->Plot(15U, 0xFFFF); break;
		default:
		case CharacterCell::CURLY_UNDERLINE:	handle->Plot(14U, 0x6666); handle->Plot(15U, 0x9999); break;
		case CharacterCell::LCURLY_UNDERLINE:	handle->Plot(14U, 0x3C3C); handle->Plot(15U, 0xC3C3); break;
		case CharacterCell::DOTTED_UNDERLINE:	handle->Plot(15U, 0xAAAA); break;
		case CharacterCell::LDOTTED_UNDERLINE:	handle->Plot(15U, 0x8181); break;
		case CharacterCell::LLDOTTED_UNDERLINE:	handle->Plot(15U, 0x8001); break;
		case CharacterCell::DASHED_UNDERLINE:	handle->Plot(15U, 0x9999); break;
		case CharacterCell::LDASHED_UNDERLINE:	handle->Plot(15U, 0xC3C3); break;
		case CharacterCell::LLDASHED_UNDERLINE:	handle->Plot(15U, 0xF00F); break;
	}
	if (attributes & CharacterCell::STRIKETHROUGH) {
		handle->Plot(7U, 0xFFFF);
		handle->Plot(8U, 0xFFFF);
	}
	if (attributes & CharacterCell::OVERLINE) {
		handle->Plot(0U, 0xFFFF);
	}
	if (attributes & CharacterCell::INVERSE) {
		for (unsigned row(0U); row < 16U; ++row) handle->Plot(row, ~handle->Row(row));
	}
	if (attributes & CharacterCell::FRAME) {
		handle->Plot(0U, handle->Row(0U) ^ 0xFFFF);
		for (unsigned row(1U); row < 15U; ++row) handle->Plot(row, handle->Row(row) ^ 0x8001);
		handle->Plot(15U, handle->Row(15U) ^ 0xFFFF);
	}
	if (attributes & CharacterCell::ENCIRCLE) {
		handle->Plot(0U, handle->Row(0U) ^ 0x3FFC);
		handle->Plot(1U, handle->Row(1U) ^ 0x4002);
		for (unsigned row(2U); row < 14U; ++row) handle->Plot(row, handle->Row(row) ^ 0x8001);
		handle->Plot(14U, handle->Row(14U) ^ 0x4002);
		handle->Plot(15U, handle->Row(15U) ^ 0x3FFC);
	}
}

namespace {
	// These are just enough characters to make the login-envuidgid screen look sane, enough to realize that there's a missing font problem.
	const uint16_t greeks[7][16] = {
		{
			0xFFFF,	// ****************
			0xFFFF,	// ****************
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xFFFF,	// ****************
			0xFFFF,	// ****************
		}, {
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x3FFC,	// --************--
			0x0000,	// ----------------
			0x0000,	// ----------------
		}, {
			0xFFFF,	// ****************
			0xFFFF,	// ****************
			0x0000,	// ----------------
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0xFFFF,	// ****************
		}, {
			0xFFFF,	// ****************
			0xFFFF,	// ****************
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC3C3, // **----****----**
			0xC3C3, // **----****----**
			0xC3C3, // **----****----**
			0xC3C3, // **----****----**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xC003, // **------------**
			0xFFFF,	// ****************
			0xFFFF,	// ****************
		}, {
			0x0003,	// --------------**
			0x0003,	// --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0xFFFF,	// ****************
			0xFFFF,	// ****************
		}, {
			0x0003,	// --------------**
			0x0003,	// --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
			0x0003, // --------------**
		}, {
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0x0000,	// ----------------
			0xFFFF,	// ****************
			0xFFFF,	// ****************
		}
	};
	inline
	void
	PlotGreekTo(
		GraphicsInterface::GlyphBitmapHandle handle,
		std::size_t index
	) {
		for (unsigned row(0U); row < 16U; ++row) handle->Plot(row, greeks[index][row]);
	}
}

void
GraphicsInterface::PlotGreek(GlyphBitmapHandle handle, uint32_t character)
{
	if (0x20 > character || (0xA0 > character && 0x80 <= character)) {
		// C0 and C1 control characters are boxes.
		PlotGreekTo(handle, 0U);
	} else switch (character)
	{
		case 0x000020: case 0x0000A0: case 0x00200B:
			// Whitespace is blank.
			for (unsigned row(0U); row < 16U; ++row) handle->Plot(row, 0x0000);
			break;
		case 0x01FB81: PlotGreekTo(handle, 2U); break;	// MouseText title bar
		case 0x01FBBC: PlotGreekTo(handle, 3U); break;	// MouseText close box
		case 0x01FB7F: PlotGreekTo(handle, 4U); break;	// MouseText sizer box
		case 0x002595: PlotGreekTo(handle, 5U); break;	// MouseText left edge
		case 0x002581: PlotGreekTo(handle, 6U); break;	// MouseText bottom edge
		// Everything else is greeked to a rectangle.
		default: PlotGreekTo(handle, 1U); break;
	}
}

void
GraphicsInterface::BitBLT(ScreenBitmapHandle s, GlyphBitmapHandle g, unsigned short y, unsigned short x, const ColourPair & colour)
{
	for (unsigned row(0U); row < 16U; ++row) {
		const uint16_t bits(g->Row(row));
		s->Plot(y + row, x, bits, colour);
	}
}

void
GraphicsInterface::BitBLTMask(ScreenBitmapHandle s, GlyphBitmapHandle g, GlyphBitmapHandle m, unsigned short y, unsigned short x, const ColourPair colours[2])
{
	for (unsigned row(0U); row < 16U; ++row) {
		const uint16_t bits(g->Row(row));
		const uint16_t mask(m->Row(row));
		s->Plot(y + row, x, bits, mask, colours);
	}
}

void
GraphicsInterface::BitBLTAlpha(ScreenBitmapHandle s, GlyphBitmapHandle g, unsigned short y, unsigned short x, const CharacterCell::colour_type & colour)
{
	for (unsigned row(0U); row < 16U; ++row) {
		const uint16_t bits(g->Row(row));
		s->AlphaBlend(y + row, x, bits, colour);
	}
}

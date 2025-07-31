/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_GRAPHICSINTERFACE_H)
#define INCLUDE_GRAPHICSINTERFACE_H

#include <cstddef>
#include <stdint.h>
#include "CharacterCell.h"

class GraphicsInterface {
public:
	GraphicsInterface() {}
	~GraphicsInterface() {}

	typedef unsigned short PixelCoordinate;

	/// \brief the abstract screen bitmap
	struct ScreenBitmap {
		ScreenBitmap(GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, unsigned short d) : yres(y), xres(x), depth(d) {}
		virtual ~ScreenBitmap() = 0;

		virtual void Plot (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, const ColourPair & colour) = 0;
		virtual void Plot (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, uint16_t mask, const ColourPair colours[2]) = 0;
		virtual void AlphaBlend (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, const CharacterCell::colour_type & colour) = 0;
	protected:
		const GraphicsInterface::PixelCoordinate yres, xres;
		const unsigned short depth;
	};
	typedef ScreenBitmap * ScreenBitmapHandle;

	/// \brief a screen bitmap with at least one whole row of pixels in process memory
	struct MemoryMappedScreenBitmap :
		public ScreenBitmap
	{
		MemoryMappedScreenBitmap(GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, unsigned short d) : ScreenBitmap(y, x, d) {}

		void Plot (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, const ColourPair & colour);
		void Plot (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, uint16_t mask, const ColourPair colours[2]);
		void AlphaBlend (GraphicsInterface::PixelCoordinate y, GraphicsInterface::PixelCoordinate x, uint16_t bits, const CharacterCell::colour_type & colour);
	protected:
		virtual void * GetStartOfLine(GraphicsInterface::PixelCoordinate y) = 0;
	};

	/// \brief the abstract glyph bitmap
	struct GlyphBitmap {
		virtual ~GlyphBitmap() = 0;
		virtual void Plot (std::size_t row, uint16_t bits) = 0;
		virtual uint16_t Row (std::size_t row) const = 0;
	};
	typedef GlyphBitmap * GlyphBitmapHandle;

	/// \brief a concrete glyph bitmap in system memory
	struct SystemMemoryGlyphBitmap :
		public GlyphBitmap
	{
		virtual ~SystemMemoryGlyphBitmap() {}
		uint16_t rows[16];

		void Plot (std::size_t row, uint16_t bits) { if (row < sizeof rows/sizeof *rows) rows[row] = bits; }
		uint16_t Row (std::size_t row) const { return row < sizeof rows/sizeof *rows ? rows[row] : 0U; }
	};

	void DeleteGlyphBitmap(GlyphBitmapHandle handle) { delete handle; }
	GlyphBitmapHandle MakeGlyphBitmap() { return new SystemMemoryGlyphBitmap(); }

	void ApplyAttributesToGlyphBitmap(GlyphBitmapHandle handle, CharacterCell::attribute_type attributes);
	void PlotGreek(GlyphBitmapHandle handle, uint32_t character);

	void BitBLT(ScreenBitmapHandle, GlyphBitmapHandle, PixelCoordinate y, PixelCoordinate x, const ColourPair & colour);
	void BitBLTMask(ScreenBitmapHandle, GlyphBitmapHandle, GlyphBitmapHandle, PixelCoordinate y, PixelCoordinate x, const ColourPair colours[2]);
	void BitBLTAlpha(ScreenBitmapHandle, GlyphBitmapHandle, PixelCoordinate y, PixelCoordinate x, const CharacterCell::colour_type & colour);

};

#endif

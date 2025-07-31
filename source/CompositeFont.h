/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_COMPOSITEFONT_H)
#define INCLUDE_COMPOSITEFONT_H

#include <vector>
#include <list>
#include <stdint.h>
#include <unistd.h>
#include <cstddef>
#include "Monospace16x16Font.h"
#include "FileDescriptorOwner.h"

/// \brief A font that is under the covers a composite of multiple different font file providers
class CombinedFont :
	public Monospace16x16Font
{
public:
	/// \brief the abstract base class for font file providers
	struct Font {
		/// \brief a mapping from a range of Unicode code points to a range of font glyphs
		struct UnicodeMapEntry {
			uint32_t codepoint;
			std::size_t glyph_number, count;
			bool operator < ( const UnicodeMapEntry & ) const ;
			bool Contains (uint32_t) const;
		};
		/// \brief a Unicode map is an ordered set of such ranges that can be searched with std::lower_bound<>
		typedef std::vector<UnicodeMapEntry> UnicodeMap;

		enum Weight { LIGHT, MEDIUM, DEMIBOLD, BOLD, NUM_WEIGHTS };
		enum Slant { UPRIGHT, ITALIC, OBLIQUE, NUM_SLANTS };

		Font ( Slant s ) : slant(s) {}
		virtual ~Font() = 0;

		Slant query_slant() const { return slant; }

		virtual bool Read(uint32_t, Weight weight, uint16_t b[16], unsigned short & h, unsigned short & w) = 0;
	protected:
		Slant slant;
		static void AddMapping(UnicodeMap & unicode_map, uint32_t character, std::size_t glyph_number, std::size_t count);
	};
	/// \brief the base class for memory mapped fonts
	struct MemoryMappedFont : public Font {
		MemoryMappedFont(Slant s, unsigned short h, unsigned short w, const void * b, const void * rb, std::size_t z, std::size_t o) : Font(s), base(b), real_base(rb), size(z), offset(o), height(h), width(w) {}
	protected:
		const void * const base, * const real_base;
		const std::size_t size, offset;
		const unsigned short height, width;
		~MemoryMappedFont();
	};
	/// \brief a simple straight in-memory font that owns a memory mapping of a file
	struct RawFileFont : public MemoryMappedFont {
		RawFileFont(Weight w, Slant s, unsigned short y, unsigned short x, const void * b, std::size_t z, std::size_t o) : MemoryMappedFont(s, y, x, b, b, z, o), weight(w) {}
		void AddMapping(uint32_t c, std::size_t g, std::size_t l) { Font::AddMapping(unicode_map, c, g, l); }
	protected:
		UnicodeMap unicode_map;
		const Weight weight;
		virtual bool Read(uint32_t, Weight weight, uint16_t b[16], unsigned short &, unsigned short &);
	};
	/// \brief the abstract base of an out-of-line font in a vtfont file
	struct VTFileFont : public MemoryMappedFont {
		VTFileFont(bool f, Slant s, unsigned short y, unsigned short x, const void * b, const void * rb, std::size_t z, std::size_t o) : MemoryMappedFont(s, y, x, b, rb, z, o), faint(f) {}
		void AddMapping(std::size_t index, uint32_t c, std::size_t g, std::size_t l) { Font::AddMapping(unicode_maps[index], c, g, l); }
	protected:
		const bool faint;
		UnicodeMap unicode_maps[4];
		bool CheckWeight(Weight, std::size_t &);
	};
	/// \brief an out-of-line font in a vtfont file
	struct LeftVTFileFont : public VTFileFont {
		LeftVTFileFont(bool f, Slant s, unsigned short h, unsigned short w, const void * b, const void * rb, std::size_t z, std::size_t o) : VTFileFont(f, s, h, w, b, rb, z, o) {}
	protected:
		virtual bool Read(uint32_t, Weight, uint16_t b[16], unsigned short &, unsigned short &);
	};
	/// \brief an out-of-line font in a vtfont file, 16 bits wide
	struct LeftRightVTFileFont : public VTFileFont {
		LeftRightVTFileFont(bool f, Slant s, unsigned short y, unsigned short x, const void * b, const void * rb, std::size_t z, std::size_t o) : VTFileFont(f, s, y, x, b, rb, z, o) {}
	protected:
		virtual bool Read(uint32_t, Weight, uint16_t b[16], unsigned short &, unsigned short &);
	};

	~CombinedFont();

	RawFileFont * AddRawFileFont(Font::Weight, Font::Slant, unsigned short y, unsigned short x, const void * b, std::size_t z, std::size_t o);
	LeftVTFileFont * AddLeftVTFileFont(bool, Font::Slant, unsigned short y, unsigned short x, const void * b, const void * rb, std::size_t z, std::size_t o);
	LeftRightVTFileFont * AddLeftRightVTFileFont(bool, Font::Slant, unsigned short y, unsigned short x, const void * b, const void * rb, std::size_t z, std::size_t o);

	virtual const uint16_t * ReadGlyph (uint32_t character, bool bold, bool faint, bool italic);
protected:
	typedef std::list<Font *> FontList;
	FontList fonts;
	uint16_t synthetic[16];

	const uint16_t * ReadGlyph (Font &, uint32_t character, Font::Weight w, bool synthesize_bold, bool synthesize_oblique);
	const uint16_t * ReadGlyph (uint32_t character, Font::Weight w, Font::Slant s, bool synthesize_bold, bool synthesize_oblique);
};

#endif

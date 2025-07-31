/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_TUIDISPLAYCOMPOSITOR_H)
#define INCLUDE_TUIDISPLAYCOMPOSITOR_H

#include <vector>
#include "CharacterCell.h"

/// \brief Output composition and change buffering for a TUI
///
/// This implements a "new" array and a "cur" array.
/// The output is composed into the "new" array and then transposed into the "cur" array.
/// Entries in the "cur" array have an additional "touched" flag to indicate that they were changed during transposition.
/// Actually outputting the "cur" array is the job of another class; this class knows nothing about I/O.
/// Layered on top of this are VIO and other access methods.
class TUIDisplayCompositor
{
public:
	typedef unsigned short coordinate;
	struct xy {
		coordinate x, y;
		xy(coordinate xp, coordinate yp) : x(xp), y(yp) {}
		xy() : x(0U), y(0U) {}
	} ;
	struct xyz : public xy {
		coordinate z;
		xyz(coordinate xp, coordinate yp, coordinate zp) : xy(xp, yp), z(zp) {}
		xyz() : xy(), z(0U) {}
	} ;
	struct wh {
		coordinate w, h;
		wh(coordinate wp, coordinate hp) : w(wp), h(hp) {}
		wh() : w(0U), h(0U) {}
	} ;

	TUIDisplayCompositor(bool invalidate_software_cursor, coordinate h, coordinate w);
	~TUIDisplayCompositor();
	coordinate query_h() const { return size.h; }
	coordinate query_w() const { return size.w; }
	coordinate query_cursor_col() const { return cursor.x; }
	coordinate query_cursor_row() const { return cursor.y; }
	coordinate query_pointer_col() const { return pointer.x; }
	coordinate query_pointer_row() const { return pointer.y; }
	coordinate query_pointer_dep() const { return pointer.z; }
	void touch_all();
	void touch_width_change_shadows();
	void repaint_new_to_cur();
	void poke(coordinate y, coordinate x, const CharacterCell & c);
	void move_cursor(coordinate y, coordinate x);
	bool change_pointer_col(coordinate col);
	bool change_pointer_row(coordinate row);
	bool change_pointer_dep(coordinate dep);
	bool is_marked(bool inclusive, coordinate y, coordinate x);
	bool is_pointer(coordinate y, coordinate x);
	void set_cursor_state(CursorSprite::attribute_type a, CursorSprite::glyph_type g);
	void set_pointer_attributes(PointerSprite::attribute_type a);
	bool set_screen_flags(ScreenFlags::flag_type f);
	CursorSprite::glyph_type query_cursor_glyph() const { return cursor_glyph; }
	CursorSprite::attribute_type query_cursor_attributes() const { return cursor_attributes; }
	PointerSprite::attribute_type query_pointer_attributes() const { return pointer_attributes; }
	ScreenFlags::flag_type query_screen_flags() const { return screen_flags; }
	void resize(coordinate h, coordinate w);
	void scroll_up(coordinate h);
	void scroll_down(coordinate h);

	class DirtiableCell : public CharacterCell {
	public:
		DirtiableCell(character_type c, attribute_type a, const ColourPair & p) : CharacterCell(c, a, p), t(false) {}
		DirtiableCell() : CharacterCell(), t(true) {}
		bool touched() const { return t; }
		void untouch() { t = false; }
		void touch() { t = true; }
		DirtiableCell & operator = ( const CharacterCell & c );
		DirtiableCell & operator = ( const DirtiableCell & c );
	protected:
		bool t;
	};

	DirtiableCell & cur_at(coordinate y, coordinate x) { return cur_cells[static_cast<std::size_t>(y) * size.w + x]; }
	CharacterCell & new_at(coordinate y, coordinate x) { return new_cells[static_cast<std::size_t>(y) * size.w + x]; }
protected:
	bool invalidate_software_cursor;
	struct xy cursor;
	struct xyz pointer;
	CursorSprite::glyph_type cursor_glyph;
	CursorSprite::attribute_type cursor_attributes;
	PointerSprite::attribute_type pointer_attributes;
	ScreenFlags::flag_type screen_flags;
	struct wh size;
	std::vector<DirtiableCell> cur_cells;
	std::vector<CharacterCell> new_cells;
};

#endif

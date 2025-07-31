/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstddef>
#include "UnicodeClassification.h"
#include "CharacterCell.h"
#include "TUIDisplayCompositor.h"

/* Character cell comparison ************************************************
// **************************************************************************
*/

namespace {

inline
bool
operator != (
	const CharacterCell & a,
	const CharacterCell & b
) {
	return a.character != b.character || a.attributes != b.attributes || a.foreground != b.foreground || a.background != b.background;
}

inline
unsigned
width (
	uint32_t ch
) {

	if (0x000000AD == ch) return 1U;
	if ((0x00001160 <= ch && ch <= 0x000011FF)
	||  UnicodeCategorization::IsMarkEnclosing(ch)
	||  UnicodeCategorization::IsMarkNonSpacing(ch)
	||  UnicodeCategorization::IsOtherFormat(ch)
	||  UnicodeCategorization::IsOtherSurrogate(ch)
	||  UnicodeCategorization::IsOtherControl(ch)
	)
		return 0U;
	if (UnicodeCategorization::IsWideOrFull(ch))
		return 2U;
	return 1U;
}

}

/* The TUIDisplayCompositor class *******************************************
// **************************************************************************
*/

TUIDisplayCompositor::TUIDisplayCompositor(
	bool i,
	coordinate init_h,
	coordinate init_w
) :
	invalidate_software_cursor(i),
	cursor(),
	pointer(),
	cursor_glyph(CursorSprite::BLOCK),
	cursor_attributes(0),	// Initialize to invisible so that making it visible causes a repaint.
	pointer_attributes(0),	// Initialize to invisible so that making it visible causes a repaint.
	screen_flags(0),
	size(init_w, init_h),
	cur_cells(static_cast<std::size_t>(size.h) * size.w),
	new_cells(static_cast<std::size_t>(size.h) * size.w)
{
}

TUIDisplayCompositor::DirtiableCell &
TUIDisplayCompositor::DirtiableCell::operator = ( const CharacterCell & c )
{
	if (c != *this) {
		CharacterCell::operator =(c);
		t = true;
	}
	return *this;
}
TUIDisplayCompositor::DirtiableCell &
TUIDisplayCompositor::DirtiableCell::operator = ( const DirtiableCell & c )
{
	CharacterCell::operator =(c);
	t = c.t;
	return *this;
}

TUIDisplayCompositor::~TUIDisplayCompositor()
{
}

void
TUIDisplayCompositor::resize(
	coordinate new_h,
	coordinate new_w
) {
	if (size.h == new_h && size.w == new_w) return;
	touch_all();
	size.h = new_h;
	size.w = new_w;
	const std::size_t s(static_cast<std::size_t>(size.h) * size.w);
	if (cur_cells.size() != s) cur_cells.resize(s);
	if (new_cells.size() != s) new_cells.resize(s);
}

void
TUIDisplayCompositor::scroll_up (
	coordinate h
) {
	if (h >= size.h) {
		touch_all();
		return;
	}
	for (unsigned row(size.h); row-- > h; )
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col) = cur_at(row - h, col);
	for (unsigned row(h); row-- > 0U; )
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col).touch();
}

void
TUIDisplayCompositor::scroll_down (
	coordinate h
) {
	if (h >= size.h) {
		touch_all();
		return;
	}
	for (unsigned row(0U); row < size.h - h; ++row)
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col) = cur_at(row + h, col);
	for (unsigned row(size.h - h); row < size.h; ++row)
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col).touch();
}

void
TUIDisplayCompositor::touch_all()
{
	for (unsigned row(0); row < size.h; ++row)
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col).touch();
}

void
TUIDisplayCompositor::touch_width_change_shadows()
{
	for (unsigned row(0); row < size.h; ++row)
		for (unsigned col(0); col < size.w; ++col) {
			const DirtiableCell & c(cur_at(row, col));
			const CharacterCell & n(new_at(row, col));
			if (c.character != n.character) {
				const unsigned cw(width(c.character));
				const unsigned nw(width(n.character));
				for (unsigned i(nw); i < cw && col + i < size.w; ++i)
					cur_at(row, col + i).touch();
			}
		}
}

void
TUIDisplayCompositor::repaint_new_to_cur()
{
	for (unsigned row(0); row < size.h; ++row)
		for (unsigned col(0); col < size.w; ++col)
			cur_at(row, col) = new_at(row, col);
}

void
TUIDisplayCompositor::poke(coordinate y, coordinate x, const CharacterCell & c)
{
	if (y < size.h && x < size.w) new_at(y, x) = c;
}

void
TUIDisplayCompositor::move_cursor(coordinate row, coordinate col)
{
	if (cursor.y != row || cursor.x != col) {
		if (invalidate_software_cursor) cur_at(cursor.y, cursor.x).touch();
		cursor.y = row;
		cursor.x = col;
		if (invalidate_software_cursor) cur_at(cursor.y, cursor.x).touch();
	}
}

bool
TUIDisplayCompositor::change_pointer_col(coordinate col)
{
	if (col < size.w && pointer.x != col) {
		cur_at(pointer.y, pointer.x).touch();
		pointer.x = col;
		cur_at(pointer.y, pointer.x).touch();
		return true;
	}
	return false;
}

bool
TUIDisplayCompositor::change_pointer_row(coordinate row)
{
	if (row < size.h && pointer.y != row) {
		cur_at(pointer.y, pointer.x).touch();
		pointer.y = row;
		cur_at(pointer.y, pointer.x).touch();
		return true;
	}
	return false;
}

bool
TUIDisplayCompositor::change_pointer_dep(coordinate dep)
{
	if (pointer.z != dep) {
		cur_at(pointer.y, pointer.x).touch();
		pointer.z = dep;
		cur_at(pointer.y, pointer.x).touch();
		return true;
	}
	return false;
}

void
TUIDisplayCompositor::set_cursor_state(CursorSprite::attribute_type a, CursorSprite::glyph_type g)
{
	if (cursor_attributes != a || cursor_glyph != g) {
		cursor_attributes = a;
		cursor_glyph = g;
		if (invalidate_software_cursor) cur_at(cursor.y, cursor.x).touch();
	}
}

void
TUIDisplayCompositor::set_pointer_attributes(PointerSprite::attribute_type a)
{
	if (pointer_attributes != a) {
		pointer_attributes = a;
		cur_at(pointer.y, pointer.x).touch();
	}
}

bool
TUIDisplayCompositor::set_screen_flags(ScreenFlags::flag_type f)
{
	if (screen_flags != f) {
		screen_flags = f;
		return true;
	} else
		return false;
}

/// This is a fairly minimal function for testing whether a particular cell position is within the current cursor, so that it can be displayed marked.
/// \todo TODO: When we gain mark/copy functionality, the cursor will be the entire marked region rather than just one cell.
bool
TUIDisplayCompositor::is_marked(bool inclusive, coordinate row, coordinate col)
{
	return inclusive && cursor.y == row && cursor.x == col;
}

bool
TUIDisplayCompositor::is_pointer(coordinate row, coordinate col)
{
	return pointer.y == row && pointer.x == col;
}

/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstddef>
#include <cstdarg>
#include <cstdlib>
#include <ctime>
#include <string>
#include "u32string.h"
#include "CharacterCell.h"
#include "TUIOutputBase.h"
#include "TUIVIO.h"
#include "TUIVIOWidgets.h"

/* Widget characters ********************************************************
// **************************************************************************
*/

namespace {

const ColourPair window_colour_pair(ColourPair(CharacterCell::colour_type(ALPHA_FOR_DEFAULT,0xC0,0xC0,0xC0), CharacterCell::colour_type(ALPHA_FOR_DEFAULT,0U,0U,0U)));

const CharacterCell::attribute_type border_attribute_choices[TUIOutputBase::Options::TUI_LEVELS] =	{ 0, 0, CharacterCell::INVERSE };
const uint_fast32_t title_bar_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FB81, 0x25A4, '=' };
const uint_fast32_t left_edge_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x002595, 0x2595, '|' };
const uint_fast32_t right_edge_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x002595, 0x2595, '|' };
const uint_fast32_t bottom_edge_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x002581, 0x2581, '_' };
const uint_fast32_t close_box_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBBC, 0x25A3, 'X' };
const uint_fast32_t sizer_box_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBBC, 0x25A8, '%' };
const uint_fast32_t black_box_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x002588, 0x2588, SPC };
const uint_fast32_t right_bar_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FB90, 0x2592, '#' };
const uint_fast32_t bottom_bar_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FB90, 0x2592, '#' };
const uint_fast32_t subbottom_edge_choices[TUIOutputBase::Options::TUI_LEVELS] =	{ 0x002594, 0x2594, '~' };
const uint_fast32_t br_corner_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FB7F, 0x25E2, '/' };
const uint_fast32_t left_arrow_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBB5, 0x2B60, '<' };
const uint_fast32_t right_arrow_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBB6, 0x2B62, '>' };
const uint_fast32_t up_arrow_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBB8, 0x2B61, '^' };
const uint_fast32_t down_arrow_choices[TUIOutputBase::Options::TUI_LEVELS] =		{ 0x01FBB7, 0x2B63, 'V' };

}

/* TUIVIO drawable widget base class *****************************************
// **************************************************************************
*/

TUIDrawable::TUIDrawable(const TUIOutputBase::Options &, TUIVIO & v) :
	vio(v)
{
}

TUIDrawable::~TUIDrawable() {}

/* Other TUIVIO drawable widgets ********************************************
// **************************************************************************
*/

TUIBackdrop::TUIBackdrop(const TUIOutputBase::Options & options, TUIVIO & v) :
	TUIDrawable(options, v)
{
}

void TUIBackdrop::Paint(
) {
	vio.CLSToSpace(ColourPair::def);
}

TUIWindow::TUIWindow(const TUIOutputBase::Options & options, TUIVIO & v, long px, long py, long pw, long ph) :
	TUIDrawable(options, v),
	x(px),
	y(py),
	w(pw),
	h(ph)
{
}

TUIFrame::TUIFrame(const TUIOutputBase::Options & options, TUIVIO & v, long x, long y, long w, long h) :
	TUIWindow(options, v, x, y, w, h),
	border_attribute(border_attribute_choices[options.tui_level]),
	title_bar(title_bar_choices[options.tui_level]),
	left_edge(left_edge_choices[options.tui_level]),
	right_edge(right_edge_choices[options.tui_level]),
	bottom_edge(bottom_edge_choices[options.tui_level]),
	right_bar(right_bar_choices[options.tui_level]),
	bottom_bar(bottom_bar_choices[options.tui_level]),
	subbottom_edge(subbottom_edge_choices[options.tui_level]),
	br_corner(br_corner_choices[options.tui_level]),
	close_box(close_box_choices[options.tui_level]),
	sizer_box(sizer_box_choices[options.tui_level]),
	black_box(black_box_choices[options.tui_level]),
	left_arrow(left_arrow_choices[options.tui_level]),
	right_arrow(right_arrow_choices[options.tui_level]),
	up_arrow(up_arrow_choices[options.tui_level]),
	down_arrow(down_arrow_choices[options.tui_level])
{
}

void TUIFrame::Paint(
) {
	if (h < 1L || w < 1L) return;
	// titlebar
	{
		const long this_row(y);
		if (1L < w) {
			vio.WriteNCharsAttr(this_row, x + 0, border_attribute, window_colour_pair, left_edge, 1U);
			vio.WriteNCharsAttr(this_row, x + 1, border_attribute, window_colour_pair, close_box, 1U);
			if (2L < w) {
				vio.WriteNCharsAttr(this_row, x + 2, border_attribute, window_colour_pair, title_bar, w - 2);
				if (4L < w) {
					const std::size_t l(title.length() < static_cast<std::size_t>(w - 5) ? title.length() : w - 5);
					vio.WriteCharStrAttrUTF8(this_row, x + 3, border_attribute, window_colour_pair, title.c_str(), l);
				}
			}
		} else
			// Degenerate to a close box.
			vio.WriteNCharsAttr(this_row, x, border_attribute, window_colour_pair, close_box, 1U);
	}
	// middle rows and user area
	if (4L < h) {
		for (long i(1L); i + 3L < h; ++i) {
			const long this_row(y + i);
			vio.WriteNCharsAttr(this_row, x, border_attribute, window_colour_pair, left_edge, 1U);
			if (1L < w) {
				const uint_fast32_t c(3L < h && 1L == i ? up_arrow : right_bar);
				vio.WriteNCharsAttr(this_row, x + w - 1, border_attribute, window_colour_pair, c, 1U);
				if (2L < w) {
					vio.WriteNCharsAttr(this_row, x + w - 2, 0, window_colour_pair, right_edge, 1U);
					if (3L < w)
						vio.WriteNCharsAttr(this_row, x + 1, 0, window_colour_pair, SPC, w - 3);
				}
			}
		}
	}
	// bottom edge
	if (3L < h) {
		const long this_row(y + h - 3);
		vio.WriteNCharsAttr(this_row, x, border_attribute, window_colour_pair, left_edge, 1U);
		if (1L < w) {
			const uint_fast32_t c(2L < h ? down_arrow : right_bar);
			vio.WriteNCharsAttr(this_row, x + w - 1, border_attribute, window_colour_pair, c, 1U);
			if (2L < w) {
				vio.WriteNCharsAttr(this_row, x + w - 2, 0, window_colour_pair, br_corner, 1U);
				if (3L < w)
					vio.WriteNCharsAttr(this_row, x + 1, 0, window_colour_pair, bottom_edge, w - 3);
			}
		}
	}
	// scroll bar
	if (2L < h) {
		const long this_row(y + h - 2);
		if (1L < w) {
			vio.WriteNCharsAttr(this_row, x, border_attribute, window_colour_pair, left_edge, 1U);
			vio.WriteNCharsAttr(this_row, x + w - 1, border_attribute, window_colour_pair, sizer_box, 1U);
			if (2L < w) {
				vio.WriteNCharsAttr(this_row, x + w - 2, border_attribute, window_colour_pair, black_box, 1U);
				if (3L < w) {
					vio.WriteNCharsAttr(this_row, x + 1, border_attribute, window_colour_pair, bottom_bar, w - 3U);
					if (5L < w) {
						vio.WriteNCharsAttr(this_row, x + 1, border_attribute, window_colour_pair, left_arrow, 1U);
						vio.WriteNCharsAttr(this_row, x + w - 3, border_attribute, window_colour_pair, right_arrow, 1U);
					}
				}
			}
		} else
			// Degenerate to a sizer box.
			vio.WriteNCharsAttr(this_row, x, border_attribute, window_colour_pair, sizer_box, 1U);
	}
	// sub-bottom beneath scroll bar
	if (1L < h) {
		const long this_row(y + h - 1);
		vio.WriteNCharsAttr(this_row, x, 0, window_colour_pair, SPC, 1L);
		if (1L < w)
			vio.WriteNCharsAttr(this_row, x + 1, 0, window_colour_pair, subbottom_edge, w - 1U);
	}
}

TUIStatusBar::TUIStatusBar(const TUIOutputBase::Options & options, TUIVIO & v, long x, long y, long w, long h) :
	TUIWindow(options, v, x, y, w, h),
	ascii(options.tui_level > 0U),
	clock(true),
	time()
{
}

void TUIStatusBar::Paint(
) {
	if (h < 1L || w < 1L) return;
	vio.WriteNCharsAttr(y, x, ColourPairAndAttributes::INVERSE, ColourPair::def, SPC, w);
	const std::string::size_type text_utf8length(LengthAsUTF8(text));
	const std::size_t p(text_utf8length < static_cast<std::size_t>(w) ? static_cast<std::size_t>(w) - text_utf8length : 0U);
	vio.WriteCharStrAttrUTF8(y, x + p, ColourPairAndAttributes::INVERSE, ColourPair::def, text.c_str(), text.length());
	if (clock)
		vio.WriteCharStrAttrUTF8(y, x, ColourPairAndAttributes::INVERSE, ColourPair::def, time.c_str(), time.length());	// clock overprints text
	for (long i(1L); i < h; ++i)
		vio.WriteNCharsAttr(y + i, x, ColourPairAndAttributes::INVERSE, ColourPair::def, SPC, w);
}

void TUIStatusBar::show_time(
	bool c
) {
	clock = c;
}

void TUIStatusBar::set_time(
	const time_t & now
) {
	const struct std::tm tm(*localtime(&now));
	char buf[64];
	const std::size_t l(std::strftime(buf, sizeof buf, "%F %T %z", &tm));
	if (ascii)
		time = std::string(buf, l);
	else {
		time.clear();
		time.reserve(l);
		for (std::size_t i(0U);i < l; ++i) {
			const char c(buf[i]);
			switch (c) {
				default:	time.push_back(c); break;
				case '0':	time += "\U0001FBF0"; break;
				case '1':	time += "\U0001FBF1"; break;
				case '2':	time += "\U0001FBF2"; break;
				case '3':	time += "\U0001FBF3"; break;
				case '4':	time += "\U0001FBF4"; break;
				case '5':	time += "\U0001FBF5"; break;
				case '6':	time += "\U0001FBF6"; break;
				case '7':	time += "\U0001FBF7"; break;
				case '8':	time += "\U0001FBF8"; break;
				case '9':	time += "\U0001FBF9"; break;
			}
		}
	}
}

/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_TUIVIOWIDGETS_H)
#define INCLUDE_TUIVIOWIDGETS_H

#include "TUIOutputBase.h"
struct TUIVIO;

/// \brief Drawable widget on a TUIVIO
struct TUIDrawable {
	virtual void Paint() = 0;
	virtual ~TUIDrawable();
protected:
	TUIDrawable(const TUIOutputBase::Options & options, TUIVIO & v);
	TUIVIO & vio;
};

/// \brief A uniform backdrop across the entire VIO
struct TUIBackdrop : public TUIDrawable {
	TUIBackdrop(const TUIOutputBase::Options & options, TUIVIO & v);
protected:
	virtual void Paint();
};

/// \brief An abstract base for (Presentation Manager style) window widgets
struct TUIWindow : public TUIDrawable {
	long x, y, w, h;
protected:
	TUIWindow(const TUIOutputBase::Options & options, TUIVIO & v, long px, long py, long pw, long ph);
};

/// \brief A Presentation Manager style frame that fills its client area blank
struct TUIFrame : public TUIWindow {
	std::string title;
	TUIFrame(const TUIOutputBase::Options & options, TUIVIO & v, long x, long y, long w, long h);
protected:
	virtual void Paint();
	const CharacterCell::attribute_type border_attribute;
	const uint_fast32_t title_bar;
	const uint_fast32_t left_edge;
	const uint_fast32_t right_edge;
	const uint_fast32_t bottom_edge;
	const uint_fast32_t right_bar;
	const uint_fast32_t bottom_bar;
	const uint_fast32_t subbottom_edge;
	const uint_fast32_t br_corner;
	const uint_fast32_t close_box;
	const uint_fast32_t sizer_box;
	const uint_fast32_t black_box;
	const uint_fast32_t left_arrow;
	const uint_fast32_t right_arrow;
	const uint_fast32_t up_arrow;
	const uint_fast32_t down_arrow;
};

// \brief A status bar with a clock
struct TUIStatusBar : public TUIWindow {
	std::string text;
	void set_time(const time_t & now);
	void show_time(bool);
	TUIStatusBar(const TUIOutputBase::Options & options, TUIVIO & v, long x, long y, long w, long h);
protected:
	const bool ascii;
	bool clock;
	std::string time;
	virtual void Paint();
};

#endif

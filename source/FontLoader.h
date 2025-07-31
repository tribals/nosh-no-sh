/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_FONTLOADER_H)
#define INCLUDE_FONTLOADER_H

#include "CompositeFont.h"

namespace FontLoader {

void
LoadRawFont (
	const char * prog,
	const ProcessEnvironment & envs,
	CombinedFont & font,
	CombinedFont::Font::Weight weight,
	CombinedFont::Font::Slant slant,
	const char * name
) ;

void
LoadVTFont (
	const char * prog,
	const ProcessEnvironment & envs,
	CombinedFont & font,
	bool faint,
	CombinedFont::Font::Slant slant,
	const char * name
) ;

}

#endif

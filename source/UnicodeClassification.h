/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_UNICODECLASSIFICATION_H)
#define INCLUDE_UNICODECLASSIFICATION_H

#include <stdint.h>

namespace UnicodeCategorization {

extern
bool
IsMarkNonSpacing(char32_t character);

extern
bool
IsMarkEnclosing(char32_t character);

extern
bool
IsOtherFormat(char32_t character);

extern
bool
IsOtherControl(char32_t character);

extern
bool
IsOtherSurrogate(char32_t character);

extern
bool
IsWideOrFull(char32_t character);

extern
unsigned int
CombiningClass(char32_t character);

extern
bool
IsDrawing(char32_t character);

extern
bool
IsHorizontallyRepeatable(char32_t character);

extern
bool
IsASCII(char32_t character);

extern
bool
IsASCIIPrint(char32_t character);

extern
bool
IsBMP(char32_t character);

}

#endif

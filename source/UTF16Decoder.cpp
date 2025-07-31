/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <stdint.h>
#include "UTF16Decoder.h"

enum { SURROGATE_MASK = 0xFC00, SURROGATE_H = 0xDC00, SURROGATE_L = 0xD800, SHIFT = 10, BASE = 0x10000 };

UTF16Decoder::UTF16Decoder(UCS32CharacterSink & s) :
	sink(s),
	pending_h(0U)
{
}

void
UTF16Decoder::Process(uint_fast16_t c)
{
	switch (SURROGATE_MASK & c) {
		case SURROGATE_H:
			if (pending_h) {
				sink.ProcessDecodedUTF16(pending_h, true /*error*/);
				pending_h = 0U;
			}
			pending_h = c;
			break;
		case SURROGATE_L:
			if (pending_h) {
				sink.ProcessDecodedUTF16(BASE + (static_cast<uint32_t>(pending_h & ~SURROGATE_MASK & 0xFFFF) << SHIFT) + static_cast<uint32_t>(c & ~SURROGATE_MASK & 0xFFFF), false /*no error*/);
				pending_h = 0U;
				break;
			}
			[[clang::fallthrough]];
		default:
			if (pending_h) {
				sink.ProcessDecodedUTF16(pending_h, true /*error*/);
				pending_h = 0U;
			}
			sink.ProcessDecodedUTF16(c & 0xFFFF, false /*no error*/);
			break;
	}
}

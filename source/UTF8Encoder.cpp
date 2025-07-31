/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <stdint.h>
#include "UTF8Encoder.h"

UTF8Encoder::UTF8Encoder(UTF8CharacterSink & s) :
	sink(s)
{
}

void
UTF8Encoder::Process(uint32_t c)
{
	if (c < 0x00000080) {
		const char s[1] = {
			static_cast<char>(c)
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	} else
	if (c < 0x00000800) {
		const char s[2] = {
			static_cast<char>(0xC0 | (0x1F & (c >> 6U))),
			static_cast<char>(0x80 | (0x3F & (c >> 0U))),
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	} else
	if (c < 0x00010000) {
		const char s[3] = {
			static_cast<char>(0xE0 | (0x0F & (c >> 12U))),
			static_cast<char>(0x80 | (0x3F & (c >> 6U))),
			static_cast<char>(0x80 | (0x3F & (c >> 0U))),
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	} else
	if (c < 0x00200000) {
		const char s[4] = {
			static_cast<char>(0xF0 | (0x07 & (c >> 18U))),
			static_cast<char>(0x80 | (0x3F & (c >> 12U))),
			static_cast<char>(0x80 | (0x3F & (c >> 6U))),
			static_cast<char>(0x80 | (0x3F & (c >> 0U))),
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	} else
	if (c < 0x04000000) {
		const char s[5] = {
			static_cast<char>(0xF8 | (0x03 & (c >> 24U))),
			static_cast<char>(0x80 | (0x3F & (c >> 18U))),
			static_cast<char>(0x80 | (0x3F & (c >> 12U))),
			static_cast<char>(0x80 | (0x3F & (c >> 6U))),
			static_cast<char>(0x80 | (0x3F & (c >> 0U))),
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	} else
	{
		const char s[6] = {
			static_cast<char>(0xFC | (0x01 & (c >> 30U))),
			static_cast<char>(0x80 | (0x3F & (c >> 24U))),
			static_cast<char>(0x80 | (0x3F & (c >> 18U))),
			static_cast<char>(0x80 | (0x3F & (c >> 12U))),
			static_cast<char>(0x80 | (0x3F & (c >> 6U))),
			static_cast<char>(0x80 | (0x3F & (c >> 0U))),
		};
		sink.ProcessEncodedUTF8(sizeof s, s);
	}
}

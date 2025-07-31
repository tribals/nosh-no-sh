/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_UTF8DECODER_H)
#define INCLUDE_UTF8DECODER_H

#include <stdint.h>

/// \brief Decode UTF-8 character sequences.
///
/// Characters are sent to the Process() function and the decoder outputs the decoded results to a "sink".
class UTF8Decoder
{
public:
	/// \brief The abstract base class for sinking decoded UTF-8 character sequences.
	class UCS32CharacterSink
	{
	public:
		/// \name Abstract API
		/// sink API called by the decoder, to be implemented by a derived class
		/// @{
		virtual void ProcessDecodedUTF8(char32_t character, bool decoder_error, bool overlong) = 0;
		/// @}
	protected:
		~UCS32CharacterSink() {}
	};
	UTF8Decoder(UCS32CharacterSink &);
	void Process(uint_fast8_t);
protected:
	UCS32CharacterSink & sink;
	unsigned short expected_continuation_bytes;
	uint_fast32_t assemblage, minimum;
	void SendGood();
	void SendBad();
};

#endif

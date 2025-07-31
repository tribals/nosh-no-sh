/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_UTF16DECODER_H)
#define INCLUDE_UTF16DECODER_H

#include <stdint.h>

/// \brief Decode UTF-16 character sequences.
///
/// Characters are sent to the Process() function and the decoder outputs the decoded results to a "sink".
class UTF16Decoder
{
public:
	/// \brief The abstract base class for sinking decoded UTF-16 character sequences.
	class UCS32CharacterSink
	{
	public:
		/// \name Abstract API
		/// sink API called by the decoder, to be implemented by a derived class
		/// @{
		virtual void ProcessDecodedUTF16(uint32_t character, bool decoder_error) = 0;
		/// @}
	protected:
		~UCS32CharacterSink() {}
	};
	UTF16Decoder(UCS32CharacterSink &);
	void Process(uint_fast16_t);
protected:
	UCS32CharacterSink & sink;
	uint16_t pending_h;
};

#endif

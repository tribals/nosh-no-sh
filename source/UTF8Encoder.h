/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_UTF8ENCODER_H)
#define INCLUDE_UTF8ENCODER_H

#include <stdint.h>
#include <cstddef>

/// \brief Encode UFT-8 character sequences.
///
/// Characters are sent to the Process() function and the envoder outputs the envoded results to a "sink".
class UTF8Encoder
{
public:
	/// \brief The abstract base class for sinking envoded UTF-8 character sequences.
	class UTF8CharacterSink
	{
	public:
		/// \name Abstract API
		/// sink API called by the encoder, to be implemented by a derived class
		/// @{
		virtual void ProcessEncodedUTF8(std::size_t l, const char * p) = 0;
		/// @}
	protected:
		~UTF8CharacterSink() {}
	};
	UTF8Encoder(UTF8CharacterSink &);
	void Process(uint32_t);
protected:
	UTF8CharacterSink & sink;
};

#endif

/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_VISDECODER_H)
#define INCLUDE_VISDECODER_H

#include <string>

/// \brief Decode vis(3) character sequences.
struct VisDecoder
{
	VisDecoder();
	void Begin();
	std::string Normal(char c);
	std::string End();
protected:
	char c;		///< FreeBSD's implementation makes this part of the state too.
	int unvis_state;
};

#endif

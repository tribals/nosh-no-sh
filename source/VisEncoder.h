/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_VISENCODER_H)
#define INCLUDE_VISENCODER_H

#include <string>

/// \brief Encode vis(3) character sequences.
namespace VisEncoder
{
	std::string process(const std::string &);
	std::string process_only_unsafe(const std::string &);
}

#endif

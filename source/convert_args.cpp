/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include "utils.h"

/* Argument vectors and lists ***********************************************
// **************************************************************************
*/

std::vector<const char *>
convert_args_storage (
	const std::vector<std::string> & args
) {
	std::vector<const char *> r(args.size());
	for (size_t i(args.size()); i > 0 ; ) {
		--i;
		r[i] = args[i].c_str();
	}
	return r;
}

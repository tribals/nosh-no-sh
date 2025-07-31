/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <unistd.h>

#include "popt.h"
#include "ttyname.h"
#include "TerminalCapabilities.h"
#include "ECMA48Output.h"

using namespace popt;

top_table_definition::~top_table_definition() {}
bool top_table_definition::execute(processor & proc, char c)
{
	if ('?' == c) { do_help(proc); return true; }
	return table_definition::execute(proc, c);
}
bool top_table_definition::execute(processor & proc, const char * s)
{
	if (0 == std::strcmp(s, "help")) { do_help(proc); return true; }
	if (0 == std::strcmp(s, "usage")) { do_usage(proc); return true; }
	return table_definition::execute(proc, s);
}
void top_table_definition::do_usage(processor & proc)
{
	TerminalCapabilities caps(proc.envs);
	ECMA48Output o(caps, stdout, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool do_colour(query_use_colours(proc.envs, o.fd()));
	std::string shorts("?");
	gather_combining_shorts(shorts);
	std::cout << "Usage: " << proc.name << " [-";
	if (do_colour) {
		std::cout.flush();
		o.set_underline(true);
		o.flush();
	}
	std::cout << shorts;
	if (do_colour) {
		std::cout.flush();
		o.set_underline(false);
		o.flush();
	}
	std::cout << "] [--help] [--usage] ";
	long_usage(o, do_colour);
	std::cout << arguments_description << '\n';
	proc.stop();
}
void top_table_definition::do_help(processor & proc)
{
	TerminalCapabilities caps(proc.envs);
	ECMA48Output o(caps, stdout, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool do_colour(query_use_colours(proc.envs, o.fd()));
	do_usage(proc);
	std::cout.put('\n');
	help(o, do_colour);
	proc.stop();
}

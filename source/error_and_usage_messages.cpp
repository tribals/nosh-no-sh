/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstring>
#include <unistd.h>
#include "utils.h"
#include "CharacterCell.h"
#include "ECMA48Output.h"
#include "TerminalCapabilities.h"
#include "ProcessEnvironment.h"
#include "popt.h"

/* Colour utilities *********************************************************
// **************************************************************************
*/

namespace {
inline
bool
use_colours (
	int fd
) {
	return isatty(fd);
}
}

/* Common usage messages throwing EXIT_USAGE ********************************
// **************************************************************************
*/

extern
void
die [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const popt::error & e
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(e.arg, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(e.msg, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_usage [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_usage [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_usage [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what0, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what1, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_missing_argument [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fprintf(stderr, "Missing %s.", what);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_missing_next_program [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs
) {
	die_missing_argument(prog, envs, "next program");
}

void
die_missing_variable_name [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs
) {
	die_missing_argument(prog, envs, "variable name");
}

void
die_missing_service_name [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs
) {
	die_missing_argument(prog, envs, "service name");
}

void
die_missing_directory_name [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs
) {
	die_missing_argument(prog, envs, "directory name");
}

void
die_missing_expression [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs
) {
	die_missing_argument(prog, envs, "expression");
}

void
die_missing_environment_variable [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs("Missing environment variable.", stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_USAGE);
}

void
die_unexpected_argument [[gnu::noreturn]] (
	const char * prog,
	std::vector<const char *> & args,
	const ProcessEnvironment & envs
) {
	die_usage(prog, envs, args.front(), "Unexpected argument");
}

void
die_invalid_argument [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what,
	const char * how
) {
	die_usage(prog, envs, what, how);
}

void
die_unsupported_command [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	die_usage(prog, envs, what, "Unsupported command");
}

void
die_unsupported_command [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1
) {
	die_usage(prog, envs, what0, what1, "Unsupported command");
}

void
die_unrecognized_command [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	die_usage(prog, envs, what, "Unrecognized command");
}

/* Common error messages ****************************************************
// **************************************************************************
*/

void
message_error_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	int error,
	const char * what
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("ERROR", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(std::strerror(error), stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
}

void
message_error_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	message_error_errno(prog, envs, errno, what);
}

/* Common fatal messages throwing EXIT_FAILURE ******************************
// **************************************************************************
*/

void
message_fatal_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	int error,
	const char * what
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(std::strerror(error), stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
}

void
message_fatal_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	message_fatal_errno(prog, envs, errno, what);
}

void
message_fatal_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1
) {
	const int error(errno);
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what0, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what1, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(std::strerror(error), stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
}

void
message_fatal_errno (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1,
	const char * what2
) {
	const int error(errno);
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what0, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what1, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what2, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(std::strerror(error), stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
}

void
die_errno [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	int error,
	const char * what
) {
	message_fatal_errno(prog, envs, error, what);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_errno [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what
) {
	message_fatal_errno(prog, envs, what);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_errno [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1
) {
	message_fatal_errno(prog, envs, what0, what1);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_errno [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1,
	const char * what2
) {
	message_fatal_errno(prog, envs, what0, what1, what2);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_invalid [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_invalid [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * what0,
	const char * what1,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what0, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what1, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_parser_error [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * file,
	unsigned long line,
	const char * what,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fprintf(stderr, "%s(%lu)", file, line);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fputs(what, stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_FAILURE);
}

void
die_parser_error [[gnu::noreturn]] (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * file,
	unsigned long line,
	const char * how
) {
	TerminalCapabilities caps(envs);
	ECMA48Output o(caps, stderr, true /* C1 is 7-bit aliased */, false /* C1 is not raw 8-bit */);
	const bool colours(use_colours(o.fd()));
	std::fprintf(stderr, "%s: ", prog);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_LIGHT_ORANGE));
	std::fputs("FATAL", stderr);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.SGRColour(true /* foreground */, Map256Colour(COLOUR_DARK_ORANGE1));
	std::fprintf(stderr, "%s(%lu)", file, line);
	if (colours) o.SGRColour(true /* foreground */);
	std::fputs(": ", stderr);
	if (colours) o.set_italics(true);
	std::fputs(how, stderr);
	if (colours) o.set_italics(false);
	std::fputc('\n', stderr);
	throw static_cast<int>(EXIT_FAILURE);
}

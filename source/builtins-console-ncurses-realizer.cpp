/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstddef>
#include "builtins.h"

/* Table of commands ********************************************************
// **************************************************************************
*/

// These are the built-in commands visible in the console ncurses-using utilities.

extern void builtins ( const char * &, std::vector<const char *> &, ProcessEnvironment & );
extern void console_ncurses_realizer ( const char * &, std::vector<const char *> &, ProcessEnvironment & );
extern void system_version ( const char * & , std::vector<const char *> &, ProcessEnvironment & );

const
struct command
commands[] = {
	// Terminals
	{	"builtins",			builtins			},
	{	"version",			system_version			},
	{	"console-ncurses-realizer",	console_ncurses_realizer	},
};
const std::size_t num_commands = sizeof commands/sizeof *commands;

const
struct command
personalities[] = {
// There are no extra personalities over and above the built-in commands.
};
const std::size_t num_personalities = sizeof personalities/sizeof *personalities;

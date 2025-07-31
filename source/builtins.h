/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_BUILTINS_H)
#define INCLUDE_BUILTINS_H

/* The tables of built-in commands and personalities ************************
// **************************************************************************
*/

struct ProcessEnvironment;

struct command {
	const char * name;
	void (*func) ( const char * &, std::vector<const char *> &, ProcessEnvironment & );
} ;

extern const command commands[], personalities[];
extern const std::size_t num_commands, num_personalities;

#endif

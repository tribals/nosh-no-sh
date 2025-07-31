/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include "popt.h"
#include "FileStar.h"
#include "utils.h"
#include "ProcessEnvironment.h"
#include "LoginBannerInformation.h"

/* issue file processing ****************************************************
// **************************************************************************
*/

namespace {

inline
void
write_escape (
	const LoginBannerInformation & info,
	int c
) {
	const std::time_t now(std::time(nullptr));
	switch (c) {
		case '\\':
		default:	break;
		case EOF:	std::fputc('\\', stdout); break;
		case 'S':	std::fputs(info.query_pretty_sysname().c_str(), stdout); break;
		case 's':	std::fputs(info.query_sysname(), stdout); break;
		case 'N':	std::fputs(info.query_pretty_nodename().c_str(), stdout); break;
		case 'n':	std::fputs(info.query_nodename(), stdout); break;
		case 'r':	std::fputs(info.query_release(), stdout); break;
		case 'v':	std::fputs(info.query_version(), stdout); break;
		case 'm':	std::fputs(info.query_machine(), stdout); break;
		case 'h':	if (const char * v = info.query_hostname()) std::fputs(v, stdout); break;
		case 'O':	if (const char * v = info.query_dnsdomainname()) std::fputs(v, stdout); break;
		case 'o':	if (const char * v = info.query_nisdomainname()) std::fputs(v, stdout); break;
		case 'L':	std::fputs(info.query_location().c_str(), stdout); break;
		case 'l':	if (const char * v = info.query_line()) std::fputs(v, stdout); break;
		case 'p':	std::fputs(info.query_deployment().c_str(), stdout); break;
		case 'u':
		case 'U':
			std::printf("%lu", info.query_users());
			if ('U' == c)
				std::printf(" user%s", info.query_users() != 1 ? "s" : "");
			break;
		case 'd':
		case 't':
		case 'z':
		{
			const struct std::tm tm(*localtime(&now));
			char buf[32];
			std::strftime(buf, sizeof buf, 'd' == c ? "%F" : 't' == c ? "%T" : "%z", &tm);
			std::fputs(buf, stdout);
			break;
		}
	}
}

}

/* Main function ************************************************************
// **************************************************************************
*/

void
login_banner (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = { };
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{template-file} {prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (args.empty()) {
		die_missing_argument(prog, envs, "issue file name");
	}
	const char * issue_filename(args.front());
	args.erase(args.begin());
	next_prog = arg0_of(args);

	LoginBannerInformation info(prog, envs);

	FileStar file(std::fopen(issue_filename, "r"));
	if (!file) {
		die_errno(prog, envs, issue_filename);
	}
	for (int c(std::fgetc(file)); EOF != c; c = std::fgetc(file)) {
		if ('\\' == c) {
			c = std::fgetc(file);
			write_escape(info, c);
		} else
			std::fputc(c, stdout);
	}
	std::fflush(stdout);
	tcdrain(fileno(stdout));
}

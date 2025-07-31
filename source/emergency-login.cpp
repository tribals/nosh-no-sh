/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <unistd.h>
#include <termios.h>
#if defined(__LINUX__) || defined(__linux__)
#include <shadow.h>
#endif
#include <pwd.h>
#include "popt.h"
#include "utils.h"
#include "ProcessEnvironment.h"
#include "DefaultEnvironment.h"
#include "ttyutils.h"

namespace {

termios
make_noecho (
	const termios & ti
) {
	termios t(ti);
	t.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOCTL|ECHOKE|ECHONL);
	return t;
}

const char SH[] = "sh";

}

/* Main function ************************************************************
// **************************************************************************
*/

void
emergency_login (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	try {
		popt::definition * top_table[] = {
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}
	if (!args.empty()) die_unexpected_argument(prog, args, envs);

	const char * shell(nullptr);
	struct passwd * p(getpwnam("root"));
	if (!p || p->pw_uid != 0) p = getpwuid(0);
	if (p) {
#if defined(__LINUX__) || defined(__linux__)
		struct spwd * const s(getspnam(p->pw_name));
		if (s) {
			const char * const passwd(s->sp_pwdp);
#else
			const char * const passwd(p->pw_passwd);
#endif
			if (passwd && *passwd) {
				for (;;) {
					termios original_attr;
					char pass[1024];
					std::fputs("Emergency superuser password:", stdout);
					std::fflush(stdout);
					if (0 <= tcgetattr_nointr(STDIN_FILENO, original_attr))
						tcsetattr_nointr(STDIN_FILENO, TCSADRAIN, make_noecho(original_attr));
					const char * r(std::fgets(pass, sizeof pass, stdin));
					std::putc('\n', stdout);
					std::fflush(stdout);
					tcsetattr_nointr(STDIN_FILENO, TCSADRAIN, original_attr);
					if (!r) {
						std::fprintf(stderr, "%s: FATAL: %s\n", prog, "EOF");
#if defined(__LINUX__) || defined(__linux__)
						endspent();
#endif
						endpwent();
						throw EXIT_FAILURE;
					}
					const std::size_t l(std::strlen(pass));
					if (l > 0 && '\n' == pass[l - 1]) pass[l - 1] = '\0';
					const char *encrypted(crypt(pass, passwd));
					std::memset(pass, '\0', sizeof pass);
					if (!std::strcmp(encrypted, passwd)) break;
					std::fputs("Wrong superuser password.\n", stderr);
				}
			}
#if defined(__LINUX__) || defined(__linux__)
			endspent();
		}
#endif
		if (p->pw_shell && *p->pw_shell)
			shell = strdup(p->pw_shell);
	}
	endpwent();

	if (shell && *shell) {
		execl(shell, SH, static_cast<const char *>(nullptr));
		std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, shell, std::strerror(errno));
	}

	shell = envs.query("SHELL");
	if (shell && *shell) {
		execl(shell, SH, static_cast<const char *>(nullptr));
		std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, shell, std::strerror(errno));
	}

	shell = DefaultEnvironment::UserLogin::SHELL;
	execl(shell, SH, static_cast<const char *>(nullptr));
	std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, shell, std::strerror(errno));

	shell = "/bin/sh";
	execl(shell, SH, static_cast<const char *>(nullptr));
	std::fprintf(stderr, "%s: ERROR: %s: %s\n", prog, shell, std::strerror(errno));

	args.push_back(SH);
	next_prog = arg0_of(args);
}

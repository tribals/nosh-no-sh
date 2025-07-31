/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#if defined(__LINUX__) || defined(__linux__)
#include <sys/prctl.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utils.h"
#include "fdutils.h"
#include "builtins.h"
#include "ProcessEnvironment.h"
#include "DefaultEnvironment.h"

/* Utilities ****************************************************************
// **************************************************************************
*/

namespace {

inline
const command *
find (
	const char * prog,
	bool allow_personalities
) {
	if (allow_personalities) {
		for ( const command * c(personalities); c != personalities + num_personalities; ++c )
			if (0 == strcmp(c->name, prog))
				return c;
	}
	for ( const command * c(commands); c != commands + num_commands; ++c )
		if (0 == strcmp(c->name, prog))
			return c;
	return nullptr;
}

inline
bool
internal_execve (
	const char * dir,
	const char * end,
	const char * prog,
	const char * const * args,
	const char * const * envs
) {
	std::size_t l(end - dir);
	const std::size_t proglen(std::strlen(prog));
	std::vector<char> buf((0U == l ? 1U : l) + 1U + proglen + 1U, char());
	if (0U == l)
		buf[l++] = '.';
	else
		std::memcpy(buf.data(), dir, l);
	buf[l++] = '/';

#if !defined(__OpenBSD__)
	if (false) {
fallback:
#endif
		std::memcpy(buf.data() + l, prog, proglen);
		buf[l + proglen] = '\0';
		execve(buf.data(), const_cast<char **>(args), const_cast<char **>(envs));
		// This is a trick from the FreeBSD execvp() implementation.
		// If we can stat() the name, it likely isn't a directory prefix error.
		struct stat b;
		const int saved_error(errno);
		const int rc(stat(buf.data(), &b));
		errno = saved_error;
		return 0 <= rc;
#if !defined(__OpenBSD__)
	}

	buf[l] = '\0';
	const int dfd(open_dir_at(AT_FDCWD, buf.data()));
	if (0 > dfd)
		return false;
# if defined(__LINUX__) || defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__)
	const int fd(open_non_interpreted_exec_at(dfd, prog));
# else
	const int fd(open_exec_at(dfd, prog));
# endif
	if (0 > fd) {
		const int saved_error(errno);
		close(dfd);
		errno = saved_error;
# if defined(__LINUX__) || defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__)
		if (ENOEXEC == errno)
			goto fallback;
# endif
		return false;
	}
	fexecve(fd, const_cast<char **>(args), const_cast<char **>(envs));
	const int saved_error(errno);
	close(fd);
	close(dfd);
	errno = saved_error;
	return true;
#endif
}

/// A safer form of execvp() that doesn't implicitly include the current directory in the default search path.
inline
void
safe_execvp (
	const char * prog,
	const char * const * args,
	ProcessEnvironment & envs
) {
	const char * path = envs.query("PATH");
	std::string d;
	if (!path)
		path = DefaultEnvironment::Toolkit::PATH;

	int error(ENOENT);	// The most interesting file error so far.
	for (;;) {
		const char * colon = std::strchr(path, ':');
		if (!colon) colon = std::strchr(path, '\0');
		bool is_file_error(internal_execve(path, colon, prog, args, envs.data()));
		if ((E2BIG == errno) || (ENOMEM == errno))
			return;		// These errors are always fatal.
		else if (!is_file_error)
			errno = error;	// Restore a file error and keep going.
		else if (ENOENT == errno)
			errno = error;	// Restore a more interesting file error and keep going.
		else if ((EACCES == errno) || (EPERM == errno) || (EISDIR == errno))
			error = errno;	// Save this interesting file error but keep going.
		else
			return;		// All other file errors are fatal.
		if (!*colon)
			return;
		path = colon + 1;
	}
}

inline
void
exec_terminal [[gnu::noreturn]] (
	const char * & prog,
	const char * & next_program,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	for (;;) {
		if (args.empty())
			die_missing_next_program(prog, envs);

		if (std::strchr(next_program, '/')) {
			args.push_back(nullptr);
			execve(next_program, const_cast<char **>(args.data()), const_cast<char **>(envs.data()));
			die_errno(prog, envs, next_program);
		} else if (const command * c = find(next_program, false)) {
			prog = basename_of(next_program);
			setprocname(prog);
			setprocargv(args.size(), args.data());
			setprocenvv(envs.size(), envs.data());
			c->func(next_program, args, envs);
		} else {
			args.push_back(nullptr);
			safe_execvp(next_program, args.data(), envs);
			die_errno(prog, envs, next_program);
		}
	}
}

}

/* Main function ************************************************************
// **************************************************************************
*/

int
main (
	int argc,
	const char * argv[],
	const char * envp[]
) {
	if (argc < 1) return EXIT_USAGE;
	std::vector<const char *> args(argv, argv + argc);
	ProcessEnvironment envs(envp);
	const char * next_prog(arg0_of(args));
	const char * prog(basename_of(next_prog));
	const command * c(find(prog, true));
	if (!c) die_usage(prog, envs, "Unknown command personality.");
	try {
		// If we were run via fexec() then the "comm" name will be something uninformative like "4" or "5" (from "/proc/self/fd/5").
		// Make it something informative.
		setprocname(prog);
		c->func(next_prog, args, envs);
		exec_terminal (prog, next_prog, args, envs);
	} catch (int r) {
		return r;
	}
}

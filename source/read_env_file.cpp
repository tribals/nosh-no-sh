/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "utils.h"
#include "fdutils.h"
#include "FileStar.h"

std::string
read_env_file (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * dir,
	const char * basename,
	int fd,
	bool full,
	bool chomp
) {
	std::string r;
	FileStar f(fdopen(fd, "r"));
	if (!f) {
exit_error:
		die_errno(prog, envs, dir, basename);
	}
	for (bool ltrim(chomp);;) {
		int c(std::fgetc(f));
		if (std::feof(f)) break;
		if ('\n' == c) {
			if (chomp) r = rtrim(r);
			if (!full) break;
			ltrim = chomp;
		} else {
			if ('\0' == c) c = '\n';
			if (ltrim) {
				if (std::isspace(c)) continue;
				ltrim = false;
			}
		}
		r += char(c);
	}
	if (std::ferror(f)) goto exit_error;
	return r;
}

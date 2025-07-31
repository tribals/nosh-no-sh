/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <cstdlib>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include "log_dir.h"
#include "fdutils.h"

static const std::string slash("/");

std::string
effective_user_log_dir()
{
	std::string r("/var/log/user/");
	// Do not use cuserid() here.
	// BSD libc defines L_cuserid as 17.
	// But GNU libc is even worse and defines it as a mere 9.
	if (struct passwd * p = getpwuid(geteuid()))
		r += p->pw_name;
	else
		r = "/dev/null";	// Yes, this intentionally gets a slash appended.
	endpwent();
	return r + slash;
}

/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <unistd.h>
#include "utils.h"
#include "nmount.h"
#include "common-manager.h"	// Because we make API mounts too.
#include "fdutils.h"
#include "popt.h"
#include "FileDescriptorOwner.h"

/* Main function ************************************************************
// **************************************************************************
*/

namespace {

void
mount_or_remount_readonly (
	const char * prog,
	const ProcessEnvironment & envs,
	const char * where
) {
	const FileDescriptorOwner fd(open_dir_at(AT_FDCWD, where));
	if (0 > fd.get()) {
		if (ENOENT != errno)
			die_errno(prog, envs, where);
		else
			return;
	}

	struct iovec remount_iov[] = {
		FSPATH,			{ const_cast<char *>(where), std::strlen(where) + 1 },
		FSTYPE,			MAKE_IOVEC(""),
	};
#if defined(__LINUX__) || defined(__linux__)
	const int remount_flags(MS_REMOUNT|MS_RDONLY);
#else
	const int remount_flags(MNT_UPDATE|MNT_RDONLY);
#endif

	if (0 > nmount(remount_iov, sizeof remount_iov/sizeof *remount_iov, remount_flags)) {
		if (EINVAL != errno && EBUSY != errno)
			die_errno(prog, envs, "nmount", where);
	}

	struct iovec bind_mount_iov[] = {
		FSPATH,			{ const_cast<char *>(where), std::strlen(where) + 1 },
		FROM,			{ const_cast<char *>(where), std::strlen(where) + 1 },
#if defined(__LINUX__) || defined(__linux__)
		FSTYPE,			MAKE_IOVEC(""),
#else
		FSTYPE,			MAKE_IOVEC("nullfs"),
#endif
	};
#if defined(__LINUX__) || defined(__linux__)
	const int bind_mount_flags(MS_BIND|MS_REC|MS_RDONLY);
#else
	const int bind_mount_flags(MNT_RDONLY);
#endif

	if (0 > nmount(bind_mount_iov, sizeof bind_mount_iov/sizeof *bind_mount_iov, bind_mount_flags)) {
		die_errno(prog, envs, "nmount", where);
	}
#if defined(__LINUX__) || defined(__linux__)
	const int bind_remount_flags(MS_REMOUNT|MS_BIND|MS_RDONLY);
	if (0 > nmount(remount_iov, sizeof remount_iov/sizeof *remount_iov, bind_remount_flags)) {
		die_errno(prog, envs, "nmount", where);
	}
#endif
}

}

void
make_read_only_fs (
	const char * & next_prog,
	std::vector<const char *> & args,
	ProcessEnvironment & envs
) {
	const char * prog(basename_of(args[0]));
	bool os(false), etc(false), homes(false), sysctl(false), cgroups(false);
	std::list<std::string> include_directories;
	std::list<std::string> except_directories;
	try {
		popt::bool_definition os_option('o', "os", "Make a read-only /usr.", os);
		popt::bool_definition etc_option('e', "etc", "Make a read-only /etc.", etc);
		popt::bool_definition homes_option('d', "homes", "Make a read-only /home.", homes);
		popt::bool_definition sysctl_option('s', "sysctl", "Make a read-only /sys, /proc/sys, et al..", sysctl);
		popt::bool_definition cgroups_option('c', "cgroups", "Make a read-only /sys/fs/cgroup.", cgroups);
		popt::string_list_definition include_option('i', "include", "directory", "Make this directory read-only.", include_directories);
		popt::string_list_definition except_option('x', "except", "directory", "Retain this directory as read-write.", except_directories);
		popt::definition * top_table[] = {
			&os_option,
			&etc_option,
			&homes_option,
			&sysctl_option,
			&cgroups_option,
			&include_option,
			&except_option
		};
		popt::top_table_definition main_option(sizeof top_table/sizeof *top_table, top_table, "Main options", "{prog}");

		std::vector<const char *> new_args;
		popt::arg_processor<const char **> p(args.data() + 1, args.data() + args.size(), prog, envs, main_option, new_args);
		p.process(true /* strictly options before arguments */);
		args = new_args;
		next_prog = arg0_of(args);
		if (p.stopped()) throw EXIT_SUCCESS;
	} catch (const popt::error & e) {
		die(prog, envs, e);
	}

	if (etc) {
		mount_or_remount_readonly(prog, envs, "/etc");
#if !defined(__LINUX__) && !defined(__linux__)
		mount_or_remount_readonly(prog, envs, "/usr/local/etc");
		mount_or_remount_readonly(prog, envs, "/usr/pkg/etc");
#endif
	}
	if (os) {
		mount_or_remount_readonly(prog, envs, "/usr");
		mount_or_remount_readonly(prog, envs, "/boot");
#if defined(__LINUX__) || defined(__linux__)
		mount_or_remount_readonly(prog, envs, "/efi");
#else
		mount_or_remount_readonly(prog, envs, "/lib");
		mount_or_remount_readonly(prog, envs, "/libexec");
		mount_or_remount_readonly(prog, envs, "/bin");
		mount_or_remount_readonly(prog, envs, "/sbin");
#endif
	}
	if (homes) {
		mount_or_remount_readonly(prog, envs, "/root");
#if defined(__LINUX__) || defined(__linux__)
		mount_or_remount_readonly(prog, envs, "/home");
		mount_or_remount_readonly(prog, envs, "/run/user");
#else
		mount_or_remount_readonly(prog, envs, "/usr/home");
#endif
	}
	if (sysctl) {
#if defined(__LINUX__) || defined(__linux__)
		mount_or_remount_readonly(prog, envs, "/proc/sys");
		mount_or_remount_readonly(prog, envs, "/proc/acpi");
		mount_or_remount_readonly(prog, envs, "/proc/apm");
		mount_or_remount_readonly(prog, envs, "/proc/asound");
		mount_or_remount_readonly(prog, envs, "/proc/bus");
		mount_or_remount_readonly(prog, envs, "/proc/fs");
		mount_or_remount_readonly(prog, envs, "/proc/irq");
		mount_or_remount_readonly(prog, envs, "/sys");
		mount_or_remount_readonly(prog, envs, "/sys/fs/pstore");
		mount_or_remount_readonly(prog, envs, "/sys/kernel/debug");
		mount_or_remount_readonly(prog, envs, "/sys/kernel/tracing");
#elif defined(__FreeBSD__)
		mount_or_remount_readonly(prog, envs, "/compat/linux/proc/sys");
		mount_or_remount_readonly(prog, envs, "/compat/linux/sys");
#endif
	}
	if (cgroups) {
#if defined(__LINUX__) || defined(__linux__)
		mount_or_remount_readonly(prog, envs, "/sys/fs/cgroup");
#endif
	}
	for (std::list<std::string>::const_iterator e(include_directories.end()), i(include_directories.begin()); e != i; ++i) {
		mount_or_remount_readonly(prog, envs, e->c_str());
	}
	for (std::list<std::string>::const_iterator e(except_directories.end()), i(except_directories.begin()); e != i; ++i) {
		mount_or_remount_readonly(prog, envs, e->c_str());
	}
}

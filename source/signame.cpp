/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <csignal>
#include "utils.h"

extern
const char *
classify_signal (
	int signo
) {
	switch (signo) {
		case SIGKILL:
			return "kill";
		case SIGTERM:
		case SIGINT:
		case SIGHUP:
		case SIGPIPE:
			return "term";
		case SIGABRT:
		case SIGALRM:
		case SIGQUIT:
			return "abort";
		default:
			return "crash";
	}
}

extern
const char *
signame (
	int signo
) {
	switch (signo) {
		case SIGKILL:	return "KILL";
		case SIGTERM:	return "TERM";
		case SIGINT:	return "INT";
		case SIGHUP:	return "HUP";
		case SIGPIPE:	return "PIPE";
		case SIGABRT:	return "ABRT";
		case SIGALRM:	return "ALRM";
		case SIGQUIT:	return "QUIT";
		case SIGSEGV:	return "SEGV";
		case SIGFPE:	return "FPE";
		case SIGTSTP:	return "TSTP";
		case SIGCHLD:	return "CHLD";
		case SIGTTIN:	return "TTIN";
		case SIGTTOU:	return "TTOU";
		case SIGWINCH:	return "WINCH";
#if defined(SIGINFO)
		case SIGINFO:	return "INFO";
#endif
		case SIGUSR1:	return "USR1";
		case SIGUSR2:	return "USR2";
		case SIGSTOP:	return "STOP";
		case SIGCONT:	return "CONT";
		case SIGIO:	return "IO";
		case SIGURG:	return "URG";
		default:	return nullptr;
	}
}

/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#include <unistd.h>
#include <termios.h>
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || defined(__linux__) || defined(__LINUX__)
#include <sys/ttydefaults.h>
#endif
#if defined(__linux__) || defined(__LINUX__)
#include <sys/ioctl.h>	// For struct winsize on Linux
#endif
#include "ttyutils.h"
#include "ControlCharacters.h"

namespace {

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__) || defined(__linux__) || defined(__LINUX__)
#if !defined(TTYDEF_LFLAG_NOECHO)
#define TTYDEF_LFLAG_NOECHO (TTYDEF_LFLAG&~(ECHO|ECHOE|ECHOKE|ECHOCTL))
#endif
#if !defined(TTYDEF_LFLAG_ECHO)
#define TTYDEF_LFLAG_ECHO (TTYDEF_LFLAG|(ECHO|ECHOE|ECHOKE|ECHOCTL))
#endif
#else
enum {
	CERASE		= DEL,	// ^?
	CKILL		= NAK,	// ^U
	CEOF		= EOT,	// ^D
	CINTR		= ETX,	// ^C
	CQUIT		= FS, 	// ^\ .
	CSTART		= DC1,	// ^Q
	CSTOP		= DC3,	// ^S
	CERASE2		= BS, 	// ^H
	CWERASE		= ETB,	// ^W
	CREPRINT	= DC2,	// ^R
	CSUSP		= SUB,	// ^Z
	CDSUSP		= EM, 	// ^Y
	CLNEXT		= SYN,	// ^V
	CDISCARD	= SI, 	// ^O
	CSTATUS		= DC4,	// ^T
};
enum {
	TTYDEF_IFLAG =	BRKINT|ICRNL|IMAXBEL|IXON|IXANY,
	TTYDEF_OFLAG =	OPOST|ONLCR,
	TTYDEF_LFLAG_NOECHO =	ICANON|ISIG|IEXTEN,
	TTYDEF_LFLAG_ECHO =	TTYDEF_LFLAG_NOECHO|ECHO|ECHOE|ECHOKE|ECHOCTL,
	TTYDEF_LFLAG =	TTYDEF_LFLAG_ECHO,
	TTYDEF_CFLAG =	CREAD|CS8|HUPCL,
	TTYDEF_SPEED =	B921600,
};
#endif

}

// Like the BSD cfmakesane() but more flexible and available outwith BSD.
termios
sane (
	const termios & original,
	bool no_tostop,
	bool no_local,
	bool no_utf_8,
	bool set_speed
) {
	termios t(original);

	// Unlike "stty sane", we don't set ISTRIP; it's 1970s Think.
	t.c_iflag = (TTYDEF_IFLAG&~ISTRIP)|IGNPAR|IGNBRK|IXANY;
#if defined(IUTF8)
	if (!no_utf_8)
		t.c_iflag |= IUTF8;
#else
	static_cast<void>(no_utf_8);	// Silences a compiler warning.
#endif
	t.c_oflag = TTYDEF_OFLAG;
	t.c_cflag = TTYDEF_CFLAG;
	if (!no_local)
		t.c_cflag |= CLOCAL;
	t.c_lflag = TTYDEF_LFLAG_ECHO|ECHOK;
	if (!no_tostop)
		t.c_lflag |= TOSTOP;
#if defined(_POSIX_VDISABLE)
	// See IEEE 1003.1 Interpretation Request #27 for why _POSIX_VDISABLE is not usable as a preprocessor expression.
       	if (-1 != _POSIX_VDISABLE)
		for (unsigned i(0); i < sizeof t.c_cc/sizeof *t.c_cc; ++i) t.c_cc[i] = _POSIX_VDISABLE;
	else {
#endif
	const int pd(pathconf("/", _PC_VDISABLE));
	for (unsigned i(0); i < sizeof t.c_cc/sizeof *t.c_cc; ++i) t.c_cc[i] = pd;
#if defined(_POSIX_VDISABLE)
	}
#endif
	t.c_cc[VERASE] = CERASE;
	t.c_cc[VKILL] = CKILL;
	t.c_cc[VEOF] = CEOF;
	t.c_cc[VINTR] = CINTR;
	t.c_cc[VQUIT] = CQUIT;
	t.c_cc[VSTART] = CSTART;
	t.c_cc[VSTOP] = CSTOP;
#if defined(VERASE2)
	t.c_cc[VERASE2] = CERASE2;
#endif
#if defined(VWERASE)
	t.c_cc[VWERASE] = CWERASE;
#endif
#if defined(VREPRINT)
	t.c_cc[VREPRINT] = CREPRINT;
#endif
#if defined(VSUSP)
	t.c_cc[VSUSP] = CSUSP;
#endif
#if defined(VDSUSP)
	t.c_cc[VDSUSP] = CDSUSP;
#endif
#if defined(VLNEXT)
	t.c_cc[VLNEXT] = CLNEXT;
#endif
#if defined(VDISCARD)
	t.c_cc[VDISCARD] = CDISCARD;
#endif
#if defined(VSTATUS)
	t.c_cc[VSTATUS] = CSTATUS;
#endif
	if (set_speed)
		cfsetspeed(&t, TTYDEF_SPEED);	// We don't want to accidentally hang up the terminal by setting 0 BPS.

	return t;
}

// Like the BSD cfmakesane() but more flexible and available outwith BSD.
termios
sane (
	bool no_tostop,
	bool no_local,
	bool no_utf_8
) {
	termios t = {};
	return sane(t, no_tostop, no_local, no_utf_8, true /* set default speed */);
}

/// Ignore attempts to have a zero size terminal.
void
sane (
	struct winsize & size
) {
	if (0 == size.ws_row) size.ws_row = 24;
	if (0 == size.ws_col) size.ws_col = 80;
}

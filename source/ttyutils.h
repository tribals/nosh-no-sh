/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_TTYUTILS_H)
#define INCLUDE_TTYUTILS_H

struct termios;
struct winsize;

extern
struct termios
sane (
	const struct termios & t,
	bool no_tostop,
	bool no_local,
	bool no_utf_8,
	bool set_speed
) ;
extern
struct termios
sane (
	bool no_tostop,
	bool no_local,
	bool no_utf_8
) ;
extern
struct termios
make_raw (
	const struct termios & t
) ;
extern
int
tcsetattr_nointr (
	int fd,
	int mode,
	const struct termios & t
) ;
extern
int
tcgetattr_nointr (
	int fd,
	struct termios & t
) ;
extern
void
sane (
	struct winsize & w
) ;
extern
int
tcsetwinsz_nointr (
	int fd,
	const struct winsize & w
) ;
extern
int
tcgetwinsz_nointr (
	int fd,
	struct winsize & w
) ;

#endif

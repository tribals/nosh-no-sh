#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
main="`basename "$1"`"
objects="main-exec.o builtins-${main}.o"
libraries="builtins.a utils.a"
case "`uname`" in
	Linux)
		crypt=-lcrypt
		;;
	*BSD)
		util=-lutil
		crypt=-lcrypt
		;;
esac
redo-ifchange haspam.h hasutmpx.h
grep -q -F HAS_PAM haspam.h && pam="-lpam"
redo-ifchange link ${objects} ${libraries}
exec ./link "$3" ${objects} ${libraries} ${crypt} ${static} ${util} ${pam}

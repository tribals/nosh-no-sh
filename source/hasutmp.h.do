#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange tryutmp.cpp compile link
if ( ./compile tryutmp.o tryutmp.cpp tryutmp.d && ./link tryutmp tryutmp.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_UTMPX 1' > "$3"
else
	echo '/* sysdep: -utmp */' > "$3"
fi

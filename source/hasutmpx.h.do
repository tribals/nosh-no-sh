#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange tryutmpx.cpp compile link
if ( ./compile tryutmpx.o tryutmpx.cpp tryutmpx.d && ./link tryutmpx tryutmpx.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_UTMPX 1' > "$3"
else
	echo '/* sysdep: -utmpx */' > "$3"
fi

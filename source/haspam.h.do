#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange trypam.cpp compile link
if ( ./compile trypam.o trypam.cpp tryutmpx.d && ./link trypam trypam.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_PAM 1' > "$3"
else
	echo '/* sysdep: -utmpx */' > "$3"
fi

#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange tryvis.cpp compile link
if ( ./compile tryvis.o tryvis.cpp tryvis.d && ./link tryvis tryvis.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_VIS 1' > "$3"
else
	echo '/* sysdep: -vis */' > "$3"
fi

#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange trylogincap.cpp compile link
if ( ./compile trylogincap.o trylogincap.cpp trylogincap.d && ./link trylogincap trylogincap.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_LOGINCAP 1' > "$3"
else
	echo '/* sysdep: -logincap */' > "$3"
fi

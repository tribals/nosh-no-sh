#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
redo-ifchange tryevdev.cpp compile link
if ( ./compile tryevdev.o tryevdev.cpp tryevdev.d && ./link tryevdev tryevdev.o )
	#>/dev/null 2>&1 
then
	echo '#define HAS_EVDEV 1' > "$3"
else
	echo '/* sysdep: -evdev */' > "$3"
fi

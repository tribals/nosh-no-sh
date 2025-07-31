#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:

redo-ifchange version.h systemd_names_escape_char.h hasevdev.h haslogincap.h hasnmount.h haspam.h hasutmpx.h hasutmp.h hasupdwtmpx.h hasvis.h haswscons.h

# These are the choke points of the build system.
# They have high degrees of fan-in; so we explicitly ensure that they are up-to-date before building the many things that fan into them.
# This also ensures that if the build fails in these sub-trees, it only tries one path before failing instead of failing via multiple paths.
# They have high degrees of fan-out; so we build them serially in order that we do not waste job slots on blocked overlapping sub-trees.
redo-ifchange builtins.a manager.a utils.a main-exec.o
redo-ifchange exec system-control

exec redo-ifchange all-commands all-misc all-targets all-services

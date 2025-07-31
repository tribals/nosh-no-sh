#!/bin/sh -e
## **************************************************************************
## For copyright and licensing terms, see the file named COPYING.
## **************************************************************************
# vim: set filetype=sh:
#
# Disable old static uhidd services for generic USB devices.
# This is invoked by all.do .
# 
# The new services are dynamically generated under /run .
#

for prefix in uhidd moused
do
        find "/var/local/service-bundles/services/" -maxdepth 1 -type d -name "${prefix}@*" -print0 |
        xargs -0 system-control disable --

        system-control disable /var/service-bundles/services/"${prefix}" "${prefix}".target
        system-control preset /var/service-bundles/services/"${prefix}"-log
done

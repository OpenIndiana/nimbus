#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

ORIGDIR=`pwd`
PKG_NAME=Nimbus

REQUIRED_AUTOMAKE_VERSION=1.9

if [ ! -f "$srcdir/metacity/metacity-theme-1.xml" ]; then
 echo "$srcdir doesn't look like source directory for $PKG_NAME" >&2
 exit 1
fi

. gnome-autogen.sh


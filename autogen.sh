#!/bin/sh

srcdir=`dirname $0`

which gnome-autogen.sh || {
	echo "You need gnome-common from GNOME SVN"
	exit 1
}

. gnome-autogen.sh


#!/bin/sh

#autogen.sh

aclocal
autoheader
automake --add-missing
autoconf


#!/bin/sh

# usage: path/to/make_opk.sh [<binary directory> <data directory>]
# The parameters are needed, because cmake supports out-of-source builds.
# When missing, a in-source build is assumed and default directories are set.

mydir=`dirname $0`

if [ -z "$1" ]; then
  datadir=${mydir}/../..
else
  datadir=$1
fi

cd ${datadir}
cmake .
make clean
CC=/opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc make -e CFLAGS="-Wall -O2 -std=c99 -D__GCW0__"
cd -

bin="${datadir}/freeblocks"

data="${datadir}/res"

gcwzdata="${mydir}/default.gcw0.desktop"
gcwzdata="${gcwzdata} ${mydir}/freeblocks.png"

alldata="${gcwzdata} ${bin} ${data}"

/opt/gcw0-toolchain/usr/bin/mksquashfs ${alldata} freeblocks.opk -all-root -noappend -no-exports -no-xattrs


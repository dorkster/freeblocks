#!/bin/bash

mkdir -p itch

cmake . -DCMAKE_BUILD_TYPE=Release
make clean
make

cp -r cmake itch/
cp -r res itch/
cp -r src itch/
cp CMakeLists.txt itch/
cp COPYING itch/
cp README itch/
cp pkg/itch/freeblocks.sh itch/
cp pkg/itch/.itch.toml itch/
cp freeblocks itch/freeblocks.x86-64

mkdir -p itch/x86-64
cp /usr/lib/libSDL2*.so.0 itch/x86-64/

cd itch/
tar cvzf ../freeblocks_itch.tar.gz *
cd ../
rm -rf itch/

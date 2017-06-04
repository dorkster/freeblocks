#!/bin/sh
# Launch script for <GAME>
# Questions to David Gow, see http://davidgow.net/gamecode/launch-script.html
# Set $DEBUGGER to launch the app with a debugger.

# Change to game directory
GAMEPATH="`readlink -f "$0"`"
cd "`dirname "$GAMEPATH"`"

# Set path to libraries and binary (64 bit)
LIBPATH=./x86-64
BIN=./freeblocks.x86-64

# Run the game, (optionally) with the debugger
LD_LIBRARY_PATH=$LIBPATH:$LD_LIBRARY_PATH $DEBUGGER $BIN $@

# Get the game's exit code, and return it.
e=$?
exit $e

#!/bin/sh

valgrind --leak-check=full --show-leak-kinds=definite --log-file=memcheck.txt ./freeblocks
vim memcheck.txt
rm memcheck.txt


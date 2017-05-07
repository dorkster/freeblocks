#!/bin/bash

rm -f game_html.zip
emcc src/*.c -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 --preload-file res -o index.html
# sed -i '/#output/a display:none !important;' index.html
zip -r game_html.zip index.html index.js index.data index.html.mem res/*

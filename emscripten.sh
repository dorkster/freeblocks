#!/bin/bash

mkdir -p emscripten_output
rm -f emscripten_output/game_html.zip
emcc src/*.c -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_SDL_TTF=2 --preload-file res -o emscripten_output/index.html
cp index_template.html emscripten_output/index.html
cd emscripten_output/
zip -r game_html.zip index.html index.js index.data index.html.mem ../res/*

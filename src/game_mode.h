/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2015 Cong Xu

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAME_MODE_H
#define GAME_MODE_H

#include <stdbool.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include "dork/dork_string.h"

enum {
    GAME_MODE_DEFAULT,
    GAME_MODE_JEWELS,
    GAME_MODE_DROP
};

typedef struct GameMode{
    void (*setDefaults)(void);
    int drawOffsetExtraY;
    void (*initAll)(void);
    void (*blockLogic)(void);
    SDL_Surface *background;
    void (*statusText)(char *buf, int score, int speed);
    Mix_Music *music;
    bool speed;
    void (*setCursor)(struct Cursor *cursor);
    void (*doSwitch)(struct Cursor *cursor);
    void (*bump)(struct Cursor *cursor);
    Dork_String *highscores;
}GameMode;

GameMode *game_mode;
GameMode game_mode_default;
GameMode game_mode_jewels;
GameMode game_mode_drop;

void gameModeInit();

#endif

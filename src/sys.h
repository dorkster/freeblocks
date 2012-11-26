/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2012 Justin Jacobs

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

#ifndef SYS_H
#define SYS_H

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

typedef enum { false = 0, true = 1 } bool;

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 512
#define SCREEN_BPP 32
#define FPS 60

extern SDL_Surface* screen;
extern TTF_Font* font;
extern SDL_Surface* text_info;

extern int score;
extern bool game_over;
extern bool quit;
extern int cursor_x;
extern int cursor_y;

extern int action_cooldown;
extern bool action_moveleft;
extern bool action_moveright;
extern bool action_moveup;
extern bool action_movedown;
extern bool action_switch;
extern bool action_bump;

SDL_Event event;

// Timers
unsigned int startTimer;
unsigned int endTimer;
unsigned int deltaTimer;

// Functions
bool sysInit();
bool sysLoadFiles();
void sysCleanup();
void sysInput();

// Images
extern SDL_Surface* surface_blocks;
extern SDL_Surface* surface_cursor;

#endif

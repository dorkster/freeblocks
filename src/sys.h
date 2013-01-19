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

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define FPS 60

#define TITLE_PLAY 0
#define TITLE_HIGHSCORES 1
#define TITLE_QUIT 2

SDL_Surface* screen;
TTF_Font* font;

int score;
int high_scores[10];
bool high_scores_screen;
bool game_over;
bool paused;
bool quit;
int cursor_x;
int cursor_y;
bool title_screen;
short title_option;

int action_cooldown;
bool action_left;
bool action_right;
bool action_up;
bool action_down;
bool action_switch;
bool action_bump;
bool action_pause;

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
void sysClearHighScores();

// Images
SDL_Surface* surface_blocks;
SDL_Surface* surface_clear;
SDL_Surface* surface_cursor;
SDL_Surface* surface_statusbar;
SDL_Surface* surface_background;
SDL_Surface* surface_title;
SDL_Surface* surface_highscores;

#endif

/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2012-2015 Justin Jacobs

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

#ifndef BLOCK_H
#define BLOCK_H

#include "sys.h"

#ifdef HALF_GFX
#define BLOCK_SIZE 16
#else
#define BLOCK_SIZE 32
#endif

#ifdef __JEWELS__
#define ROWS 9
#define COLS 9
#define START_ROWS ROWS
#define DISABLED_ROWS 0
#else
#define ROWS 15
#define COLS 20
#define START_ROWS 4
#define DISABLED_ROWS 1
#endif

#define DRAW_OFFSET_X ((SCREEN_WIDTH - COLS*BLOCK_SIZE) / 2)
#define DRAW_OFFSET_Y ((SCREEN_HEIGHT - ROWS*BLOCK_SIZE) / 2)
#define CLEAR_TIME 4 / (60/FPS)
#define SPEED_FACTOR 32 / BLOCK_SIZE
#define BUMP_TIME (60 / (60/FPS)) * SPEED_FACTOR
#define SPEED_TIME 1800 / (60/FPS)
#define MAX_SPEED 25
#define BLOCK_MOVE_SPEED BLOCK_SIZE / 2
#define CURSOR_MIN_Y 1
#define CURSOR_MAX_Y (ROWS-1-DISABLED_ROWS)

const int POINTS_PER_BLOCK;
const int POINTS_PER_BUMP;
const int POINTS_PER_COMBO_BLOCK;

typedef struct Block{
    int x,y;
    int dest_x, dest_y;
    bool alive;
    int color;
    bool matched;
    int clear_timer;
    int frame;
    bool moving;
}Block;

Block blocks[ROWS][COLS];
bool animating;
int bump_timer;
int bump_pixels;
int speed;
int speed_init;
int speed_timer;
int game_over_timer;

void blockSet(int i, int j, bool alive, int color);
void blockClear(int i, int j);
void blockSwitch(int i, int j, int k, int l, bool animate);
bool blockCompare(int i, int j, int k, int l);
void blockInitAll();

void blockLogic();
void blockRise();
void blockGravity();
void blockMatch();
bool blockAddLayer();
void blockSwitchCursor();

#endif

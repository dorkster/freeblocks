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
#define BLOCK_SIZE 24
#else
#define BLOCK_SIZE 48
#endif

#define CLEAR_TIME 4 / (60/FPS)
#define SPEED_FACTOR 32 / BLOCK_SIZE
#define BUMP_TIME (60 / (60/FPS)) * SPEED_FACTOR
#define SPEED_TIME 1800 / (60/FPS)
#define MAX_SPEED 25

const int POINTS_PER_BLOCK;
const int POINTS_PER_BUMP;
const int POINTS_PER_COMBO_BLOCK;

int ROWS;
int COLS;
int NUM_BLOCKS;
int START_ROWS;
int DISABLED_ROWS;
int CURSOR_MAX_X;
int CURSOR_MIN_Y;
int CURSOR_MAX_Y;
int BLOCK_MOVE_FRAMES;
int DRAW_OFFSET_X;
int DRAW_OFFSET_Y;

typedef struct Block{
    int x,y;
    int start_col, start_row;
    int dest_col, dest_row;
    bool alive;
    int color;
    bool matched;
    int clear_timer;
    int frame;
    bool moving;
    int move_counter;
    int move_counter_max;
    int return_row, return_col;
    bool sound_after_move;
}Block;

Block **blocks;
bool animating;
int bump_timer;
int bump_pixels;
int speed;
int speed_init;
int speed_timer;
int game_over_timer;
bool jewels_cursor_select;

void blockSet(int i, int j, bool alive, int color);
void blockClear(int i, int j);
void blockSwitch(int i, int j, int k, int l, bool animate, bool sound_after_move);
bool blockCompare(int i, int j, int k, int l);
void blockSetDefaults();
void blockCleanup();
void blockInitAll();

void blockLogic();
void blockRise();
void blockAddFromTop();
void blockGravity();
void blockMatch();
bool blockAddLayer();
bool blockHasSwitchMatch();
bool blockHasGaps();
void blockSwitchCursor();

#endif

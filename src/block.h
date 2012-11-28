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

#ifndef BLOCK_H
#define BLOCK_H

#include "sys.h"

#define BLOCK_SIZE 32
#define ROWS 15
#define COLS 8
#define CLEAR_TIME 30
#define BUMP_TIME 30
#define SPEED_TIME 3600
#define SPEED_PER_LEVEL 2
#define MAX_SPEED 12

const int POINTS_PER_BLOCK;

typedef struct Block{
    int x,y;
    bool alive;
    int color;
    bool matched;
}Block;

Block blocks[15][8];
int clear_delay;
int bump_timer;
int bump_pixels;
int speed;
int speed_timer;

void blockSet(int i, int j, bool alive, int color);
void blockClear(int i, int j);
void blockSwitch(int i, int j, int k, int l);
bool blockCompare(int i, int j, int k, int l);
void blockInitAll();

void blockLogic();
void blockGravity();
void blockMatch();
void blockAddLayer();

#endif

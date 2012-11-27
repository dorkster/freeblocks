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

#include "block.h"
#include "sys.h"

Block blocks[ROWS][COLS];
int clear_delay = 0;
int bump_timer = 0;
const int POINTS_PER_BLOCK = 20;

void blockSet(int i, int j, bool alive, int color) {
    blocks[i][j].x = j*BLOCK_SIZE;
    blocks[i][j].y = i*BLOCK_SIZE;
    blocks[i][j].alive = alive;
    blocks[i][j].color = color;
    blocks[i][j].matched = false;
}

void blockClear(int i, int j) {
    blocks[i][j].alive = false;
    blocks[i][j].color = 1;
    blocks[i][j].matched = false;
}

void blockSwitch(int i, int j, int k, int l) {
    if (blocks[i][j].matched || blocks[k][l].matched) return;

    int b1_color = blocks[i][j].color;
    int b1_alive = blocks[i][j].alive;
    int b2_color = blocks[k][l].color;
    int b2_alive = blocks[k][l].alive;

    blocks[i][j].color = b2_color;
    blocks[i][j].alive = b2_alive;
    blocks[k][l].color = b1_color;
    blocks[k][l].alive = b1_alive;
}

bool blockCompare(int i, int j, int k, int l) {
    if (blocks[i][j].color != blocks[k][l].color) return false;
    if (blocks[i][j].alive != blocks[k][l].alive) return false;
    return true;
}

void blockInitAll() {
    int i,j;
    int new_color = -1;
    int last_color = -1;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            blockSet(i,j,false,-1);
        }
    }

    for(i=8;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            new_color = rand() % 6;
            while (new_color == last_color || new_color == blocks[i-1][j].color) {
                new_color = rand() % 6;
            }
            last_color = new_color;
            blockSet(i,j,true,new_color);
        }
    }
}

void blockLogic() {
    blockGravity();
    blockMatch();

    if (bump_timer > 0 && clear_delay == 0) bump_timer--;
    if (bump_timer == 0) blockAddLayer();
}

void blockGravity() {
    int i,j;

    for (j=0;j<COLS;j++) {
        for (i=0;i<ROWS-1;i++) {
            if (blocks[i][j].alive && !blocks[i+1][j].alive)
                blockSwitch(i,j,i+1,j);
        }
    }
}

void blockMatch() {
    int i,j,k;
    int match_count = 0;

    // next, mark all the blocks that will be cleared
    for (i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            if (blocks[i][j].alive) {
                // horizontal matches
                match_count = 0;
                for(k=j+1;k<COLS;k++) {
                    if (blockCompare(i,j,i,k))
                        match_count++;
                    else
                        break;
                }
                if (match_count > 1) {
                    for(k=j;k<j+match_count+1;k++) {
                        if (!blocks[i][j].matched) clear_delay = CLEAR_TIME;
                        blocks[i][k].matched = true;
                    }
                }
                // vertical matches
                match_count = 0;
                for(k=i+1;k<ROWS;k++) {
                    if (blockCompare(i,j,k,j))
                        match_count++;
                    else
                        break;
                }
                if (match_count > 1) {
                    for(k=i;k<i+match_count+1;k++) {
                        if (!blocks[i][j].matched) clear_delay = CLEAR_TIME;
                        blocks[k][j].matched = true;
                    }
                }
            }
        }
    }

    if (clear_delay > 0) clear_delay--;
    if (clear_delay == 0) {
        // now, clear all the matches
        for (i=0;i<ROWS;i++) {
            for(j=0;j<COLS;j++) {
                if (blocks[i][j].matched) {
                    blockClear(i,j);
                    score += POINTS_PER_BLOCK;
                }
            }
        }
    }
}

void blockAddLayer() {
    int i,j;
    int new_color = -1;
    int last_color = -1;

    if (clear_delay > 0) return;

    // check if one of the columns is full
    // if so, set game over state
    for (j=0;j<COLS;j++) {
        if (blocks[1][j].alive) {
            game_over = true;
        }
    }

    if (cursor_y > 1) cursor_y--;

    for (j=0;j<COLS;j++) {
        for (i=1;i<ROWS;i++) {
            blockSwitch(i,j,i-1,j);
        }
    }

    for(j=0;j<COLS;j++) {
        new_color = rand() % 6;
        while (new_color == last_color || new_color == blocks[ROWS-2][j].color) {
            new_color = rand() % 6;
        }
        last_color = new_color;
        blockSet(ROWS-1,j,true,new_color);
    }

    bump_timer = BUMP_TIME;
}

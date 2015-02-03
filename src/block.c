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
const int POINTS_PER_BLOCK = 10;
const int POINTS_PER_BUMP = 5;
const int POINTS_PER_COMBO_BLOCK = 15;

void blockSet(int i, int j, bool alive, int color) {
    blocks[i][j].x = j*BLOCK_SIZE;
    blocks[i][j].y = i*BLOCK_SIZE;
    blocks[i][j].alive = alive;
    blocks[i][j].color = color;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
}

void blockClear(int i, int j) {
    blocks[i][j].alive = false;
    blocks[i][j].color = 1;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
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
    if (blocks[i][j].clear_timer > 0 || blocks[i][j].frame > 0) return false;
    if (blocks[k][l].clear_timer > 0 || blocks[k][l].frame > 0) return false;
    return true;
}

bool blockAnimate() {
    int i,j;
    bool anim = false;

    for (i=0;i<ROWS;i++) {
        for (j=0;j<COLS;j++) {
            if (blocks[i][j].matched && blocks[i][j].frame < 8) {
                if (blocks[i][j].clear_timer > 0) blocks[i][j].clear_timer--;
                if (blocks[i][j].clear_timer == 0) {
                    blocks[i][j].clear_timer = CLEAR_TIME;
                    blocks[i][j].frame++;
                }
                anim = true;
            }
        }
    }

    return anim;
}

void blockInitAll() {
    int i,j;
    int new_color = -1;
    int last_color = -1;

    animating = false;
    bump_timer = 0;
    bump_pixels = 0;
    speed = 1;
    speed_timer = SPEED_TIME;
    game_over_timer = 0;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            blockSet(i,j,false,-1);
        }
    }

    for(i=ROWS-START_ROWS;i<ROWS;i++) {
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
    animating = blockAnimate();

    blockGravity();
    blockMatch();

    if (bump_timer > 0 && !animating) bump_timer--;
    if (bump_timer == 0) {
        bump_pixels++;
        bump_timer = BUMP_TIME - speed;
        if (bump_timer < 0) bump_timer = 0;
    }
    if (bump_pixels > 0 && bump_pixels % BLOCK_SIZE == 0) {
        blockAddLayer();
        bump_pixels -= BLOCK_SIZE+1;
    }
    if (speed_timer > 0) speed_timer--;
    if (speed < MAX_SPEED && speed_timer == 0) {
        speed++;
        speed_timer = SPEED_TIME;
    }
}

void blockGravity() {
    int i,j;

    for (j=0;j<COLS;j++) {
        for (i=ROWS-2;i>0;i--) {
            if (blocks[i][j].alive && !blocks[i+1][j].alive) {
                blockSwitch(i,j,i+1,j);

                // return to the bottom of the column
                // this process will be repeated until no more blocks need to fall in this column
                i=ROWS-2;
            }
        }
    }
}

void blockMatch() {
    int i,j,k;
    int match_count = 0;
    int blocks_cleared = 0;
    bool new_match = false;

    if (!animating) {
        // now, clear all the matches
        for (i=0;i<ROWS-1;i++) {
            for(j=0;j<COLS;j++) {
                if (blocks[i][j].matched) {
                    blockClear(i,j);
                    blocks_cleared++;
                }
            }
        }
        if (blocks_cleared > 2) {
            score += blocks_cleared * POINTS_PER_BLOCK;
            if (blocks_cleared-3 > 0) score += (blocks_cleared-3) * POINTS_PER_COMBO_BLOCK;
        }
    }

    // next, mark all the blocks that will be cleared
    // skip the bottom row because blocks there aren't fully "in" the block field
    for (i=0;i<ROWS-1;i++) {
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
                        blocks[i][k].matched = true;
                        new_match = true;
                    }
                }
                // vertical matches
                match_count = 0;
                for(k=i+1;k<ROWS-1;k++) {
                    if (blockCompare(i,j,k,j))
                        match_count++;
                    else
                        break;
                }
                if (match_count > 1) {
                    for(k=i;k<i+match_count+1;k++) {
                        blocks[k][j].matched = true;
                        new_match = true;
                    }
                }
            }
        }
    }

    if (new_match) Mix_PlayChannel(-1,sound_match,0);
}

bool blockAddLayer() {
    if (animating) return false;

    int i,j;
    int new_color = -1;
    int last_color = -1;

    // check if one of the columns is full
    // if so, set game over state
    // display the "try again" menu after 2 seconds
    for (j=0;j<COLS;j++) {
        if (blocks[1][j].alive) {
            game_over_timer = FPS*2;
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

    // reset bump pixels to the previous block level
    bump_pixels -= bump_pixels % BLOCK_SIZE;

    return true;
}

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

#include "block.h"
#include "sys.h"

Block blocks[ROWS][COLS];
const int POINTS_PER_BLOCK = 10;
const int POINTS_PER_BUMP = 5;
const int POINTS_PER_COMBO_BLOCK = 15;

int speed_init = 1;

void blockSet(int i, int j, bool alive, int color) {
    blocks[i][j].x = j*BLOCK_SIZE;
    blocks[i][j].y = i*BLOCK_SIZE;
    blocks[i][j].dest_x = j*BLOCK_SIZE;
    blocks[i][j].dest_y = i*BLOCK_SIZE;
#ifdef __JEWELS__
    blocks[i][j].return_row = -1;
    blocks[i][j].return_col = -1;
#endif
    blocks[i][j].alive = alive;
    blocks[i][j].color = color;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
    blocks[i][j].moving = false;
}

void blockClear(int i, int j) {
#ifdef __JEWELS__
    blocks[i][j].return_row = -1;
    blocks[i][j].return_col = -1;
#endif
    blocks[i][j].alive = false;
    blocks[i][j].color = 1;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
    blocks[i][j].moving = false;
}

void blockSwitch(int i, int j, int k, int l, bool animate) {
    if (blocks[i][j].matched || blocks[k][l].matched) return;

    int b1_color = blocks[i][j].color;
    int b1_alive = blocks[i][j].alive;
    int b2_color = blocks[k][l].color;
    int b2_alive = blocks[k][l].alive;

    blocks[i][j].color = b2_color;
    blocks[i][j].alive = b2_alive;
    blocks[k][l].color = b1_color;
    blocks[k][l].alive = b1_alive;

    if (animate) {
        blocks[i][j].x = blocks[k][l].dest_x;
        blocks[i][j].y = blocks[k][l].dest_y;
        blocks[k][l].x = blocks[i][j].dest_x;
        blocks[k][l].y = blocks[i][j].dest_y;
    }
}

bool blockCompare(int i, int j, int k, int l) {
    if (blocks[i][j].color != blocks[k][l].color) return false;
    if (blocks[i][j].alive != blocks[k][l].alive) return false;
    if (blocks[i][j].clear_timer > 0 || blocks[i][j].frame > 0) return false;
    if (blocks[k][l].clear_timer > 0 || blocks[k][l].frame > 0) return false;
    return true;
}

int blockMatchHorizontal(int i, int j) {
    int match_count = 0;
    for(int k=j+1;k<COLS;k++) {
        if (blockCompare(i,j,i,k))
            match_count++;
        else
            break;
    }
    return match_count;
}

int blockMatchVertical(int i, int j) {
    int match_count = 0;
    for(int k=i+1;k<ROWS-DISABLED_ROWS;k++) {
        if (blockCompare(i,j,k,j))
            match_count++;
        else
            break;
    }
    return match_count;
}

bool blockAnimate() {
    int i,j;
    bool anim = false;

    for (i=0;i<ROWS;i++) {
        for (j=0;j<COLS;j++) {
            blocks[i][j].moving = false;

            if (blocks[i][j].matched && blocks[i][j].frame < 8) {
                if (blocks[i][j].clear_timer > 0) blocks[i][j].clear_timer--;
                if (blocks[i][j].clear_timer == 0) {
                    blocks[i][j].clear_timer = CLEAR_TIME;
                    blocks[i][j].frame++;
                }
                anim = true;
            }

            // move blocks
            if (blocks[i][j].dest_x < blocks[i][j].x) {
                blocks[i][j].x -= BLOCK_MOVE_SPEED;
                blocks[i][j].moving = true;
                anim = true;
            }
            else if (blocks[i][j].dest_x > blocks[i][j].x) {
                blocks[i][j].x += BLOCK_MOVE_SPEED;
                blocks[i][j].moving = true;
                anim = true;
            }

            if (blocks[i][j].dest_y < blocks[i][j].y) {
                blocks[i][j].y -= BLOCK_MOVE_SPEED;
                blocks[i][j].moving = true;
                anim = true;
            }
            else if (blocks[i][j].dest_y > blocks[i][j].y) {
                blocks[i][j].y += BLOCK_MOVE_SPEED;
                blocks[i][j].moving = true;
                anim = true;
            }
        }
    }

    return anim;
}

void blockReturn() {
    int i,j;

    for (i=0;i<ROWS;i++) {
        for (j=0;j<COLS;j++) {
            // If we attempted to switch this block but
            // there is no match, move it back
            if (blocks[i][j].dest_x == blocks[i][j].x && blocks[i][j].dest_y == blocks[i][j].y && !blocks[i][j].matched && !blocks[i][j].moving && blocks[i][j].return_row != -1) {
                blockSwitch(i, j, blocks[i][j].return_row, blocks[i][j].return_col, true);
                blocks[i][j].return_row = -1;
                blocks[i][j].return_col = -1;
            }
        }
    }
}

void blockAddLayerRandom(int i) {
    int j;
    int last_color = -1;
    for(j=0;j<COLS;j++) {
        int new_color = rand() % 6;
        while (new_color == last_color || (i > 0 && new_color == blocks[i-1][j].color)) {
            new_color = rand() % 6;
        }
        last_color = new_color;
        blockSet(i,j,true,new_color);
    }
}

bool blockHasMatches() {
    // Check if there are any matches on the board
    int i,j;

    for (i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            if (blocks[i][j].alive) {
                // horizontal matches
                if (blockMatchHorizontal(i,j) > 1) return true;
                if (blockMatchVertical(i,j) > 1) return true;
            }
        }
    }
	return false;
}

void blockInitAll() {
    int i,j;

    animating = false;
    bump_timer = 0;
    bump_pixels = 0;
    speed = speed_init;
    speed_timer = SPEED_TIME;
    game_over_timer = 0;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            blockSet(i,j,false,-1);
        }
    }

#ifndef __JEWELS__
    for(i=ROWS-START_ROWS;i<ROWS;i++) {
        blockAddLayerRandom(i);
    }
#else
    do {
        for(i=ROWS-START_ROWS;i<ROWS;i++) {
            for(j=0;j<COLS;j++) {
                blockSet(i,j,true,rand() % 6);
            }
        }
    } while (blockHasMatches());
#endif
}

void blockLogic() {
    animating = blockAnimate();

    if (animating)
        return;

    blockMatch();

#ifndef __JEWELS__
    blockRise();
#else
    blockReturn();
    blockAddFromTop();
#endif

    blockGravity();
}

void blockRise() {
    if (bump_timer > 0) bump_timer--;
    if (bump_timer == 0) {
        bump_pixels++;
        bump_timer = BUMP_TIME - (speed*SPEED_FACTOR);
        if (bump_timer < 0) bump_timer = 0;
    }
    if (bump_pixels > 0 && bump_pixels % BLOCK_SIZE == 0) {
        blockAddLayer();
        bump_pixels -= BLOCK_SIZE;
    }
    if (speed_timer > 0) speed_timer--;
    if (speed < MAX_SPEED && speed_timer == 0) {
        speed++;
        speed_timer = SPEED_TIME;
    }
}

void blockAddFromTop() {
    for(int j=0;j<COLS;j++)
        if (!blocks[0][j].alive)
            blockSet(0,j,true,rand() % 6);
}

void blockGravity() {
    int i,j,k;

    for (j=0;j<COLS;j++) {
        int gap_size = 0;
        int first_empty = -1;
        for (k=ROWS-1; k>0; k--) {
            if (first_empty == -1 && !blocks[k][j].alive)
                first_empty = k;

            if (first_empty != -1) {
                if (!blocks[k][j].alive)
                    gap_size++;
                else
                    break;
            }
        }

        if (gap_size > 0) {
            for (i=first_empty-gap_size;i>=0;i--) {
                if (blocks[i][j].alive) {
                    blockSwitch(i,j,i+gap_size,j, true);
                }
            }
        }
    }
}

void blockMatch() {
    int i,j,k;
    int match_count;
    int blocks_cleared = 0;
    bool new_match = false;

    if (!animating) {
        // now, clear all the matches
        for (i=0;i<ROWS-DISABLED_ROWS;i++) {
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
    // skip the bottom rows because blocks there aren't fully "in" the block field
    for (i=0;i<ROWS-DISABLED_ROWS;i++) {
        for(j=0;j<COLS;j++) {
            if (blocks[i][j].alive) {
                // horizontal matches
                match_count = blockMatchHorizontal(i,j);
                if (match_count > 1) {
                    for(k=j;k<j+match_count+1;k++) {
                        blocks[i][k].matched = true;
                        new_match = true;
                    }
                }
                // vertical matches
                match_count = blockMatchVertical(i,j);
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

    // check if one of the columns is full
    // if so, set game over state
    // display the "try again" menu after 2 seconds
    for (j=0;j<COLS;j++) {
        if (blocks[1][j].alive) {
            game_over_timer = FPS*2;
        }
    }

    if (cursor_y > CURSOR_MIN_Y) cursor_y--;

    for (j=0;j<COLS;j++) {
        for (i=1;i<ROWS;i++) {
            blockSwitch(i,j,i-1,j, false);
        }
    }

    blockAddLayerRandom(ROWS-1);

    // reset bump pixels to the previous block level
    bump_pixels -= bump_pixels % BLOCK_SIZE;

    return true;
}

void blockSwitchCursor(ActionMove dir) {
    int dx,dy;
    switch (dir) {
        case ACTION_LEFT: dx = -1; dy = 0; break;
        case ACTION_RIGHT: dx = 1; dy = 0; break;
        case ACTION_UP: dx = 0; dy = -1; break;
        case ACTION_DOWN: dx = 0; dy = 1; break;
        default: return;
    }
    if (cursor_x+dx < 0 || cursor_x+dx >= COLS || cursor_y+dy < 0 || cursor_y+dy >= ROWS) return;
    Block *other = &blocks[cursor_y+dy][cursor_x+dx];
    // don't allow switching blocks that are already moving
    if (blocks[cursor_y][cursor_x].moving == false && other->moving == false) {
        blockSwitch(cursor_y, cursor_x, cursor_y+dy, cursor_x+dx, true);
#ifdef __JEWELS__
        blocks[cursor_y][cursor_x].return_row = cursor_y+dy;
        blocks[cursor_y][cursor_x].return_col = cursor_x+dx;
#endif
    }
}

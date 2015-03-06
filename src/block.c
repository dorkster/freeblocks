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

const int POINTS_PER_BLOCK = 10;
const int POINTS_PER_BUMP = 5;
const int POINTS_PER_COMBO_BLOCK = 15;

int speed_init = 1;
Block **blocks = NULL;

int blockRand(void) {
    return rand() % NUM_BLOCKS;
}

void blockSet(int i, int j, bool alive, int color) {
    blocks[i][j].x = j*BLOCK_SIZE;
    blocks[i][j].y = i*BLOCK_SIZE;
    blocks[i][j].dest_x = j*BLOCK_SIZE;
    blocks[i][j].dest_y = i*BLOCK_SIZE;
    blocks[i][j].alive = alive;
    blocks[i][j].color = color;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
    blocks[i][j].moving = false;
    blocks[i][j].return_row = -1;
    blocks[i][j].return_col = -1;
    blocks[i][j].sound_after_move = false;
}

void blockClear(int i, int j) {
    blocks[i][j].alive = false;
    blocks[i][j].color = 1;
    blocks[i][j].matched = false;
    blocks[i][j].clear_timer = 0;
    blocks[i][j].frame = -1;
    blocks[i][j].moving = false;
    blocks[i][j].return_row = -1;
    blocks[i][j].return_col = -1;
    blocks[i][j].sound_after_move = false;
}

void blockSwitch(int i, int j, int k, int l, bool animate, bool sound_after_move) {
    if (i < 0 || i >= ROWS || j < 0 || j >= COLS || k < 0 || k >= ROWS || l < 0 || l >= COLS) return;
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
        blocks[i][j].sound_after_move = sound_after_move;
        blocks[k][l].x = blocks[i][j].dest_x;
        blocks[k][l].y = blocks[i][j].dest_y;
        blocks[k][l].sound_after_move = sound_after_move;
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
    bool drop_sound = false;

    for (i=0;i<ROWS;i++) {
        for (j=0;j<COLS;j++) {
            bool was_moving = blocks[i][j].moving;
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

            // play sound after block has finished moving
            if (was_moving && !blocks[i][j].moving && blocks[i][j].alive && blocks[i][j].sound_after_move) {
                drop_sound = true;
                blocks[i][j].sound_after_move = false;
            }
        }
    }

    if (drop_sound) Mix_PlayChannel(-1, sound_drop, 0);

    return anim;
}

void blockReturn() {
    int i,j;

    for (i=0;i<ROWS;i++) {
        for (j=0;j<COLS;j++) {
            // If we attempted to switch this block but
            // there is no match, move it back
            if (blocks[i][j].dest_x == blocks[i][j].x && blocks[i][j].dest_y == blocks[i][j].y && !blocks[i][j].matched && !blocks[i][j].moving && blocks[i][j].return_row != -1) {
                blockSwitch(i, j, blocks[i][j].return_row, blocks[i][j].return_col, true, false);
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
        int new_color = blockRand();
        while (new_color == last_color || (i > 0 && new_color == blocks[i-1][j].color)) {
            new_color = blockRand();
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

void blockSetDefaults() {
    blockCleanup();

    if (game_mode == GAME_MODE_JEWELS) {
        ROWS = 8;
        COLS = 8;
        NUM_BLOCKS = 7;
        START_ROWS = ROWS;
        DISABLED_ROWS = 0;
        CURSOR_MAX_X = COLS-1;
        CURSOR_MIN_Y = 0;
        BLOCK_MOVE_SPEED = BLOCK_SIZE / 4;
    }
    else {
        ROWS = 10;
        COLS = 13;
        NUM_BLOCKS = 6;
        START_ROWS = 4;
        DISABLED_ROWS = 1;
        CURSOR_MAX_X = COLS-2;
        CURSOR_MIN_Y = 1;
        BLOCK_MOVE_SPEED = BLOCK_SIZE / 2;
    }

    DRAW_OFFSET_X = (SCREEN_WIDTH - COLS * BLOCK_SIZE) / 2;
    DRAW_OFFSET_Y = (SCREEN_HEIGHT - ROWS * BLOCK_SIZE) / 2;
    CURSOR_MAX_Y = ROWS-1-DISABLED_ROWS;

    // We need to change our vertical offset if the block size != status bar size
    if (game_mode == GAME_MODE_DEFAULT) {
        DRAW_OFFSET_Y += BLOCK_SIZE-(surface_bar->h);
    }

    blocks = malloc(sizeof(Block*)*ROWS);
    for (int i=0; i<ROWS; i++) {
        blocks[i] = malloc(sizeof(Block)*COLS);
    }
}

void blockCleanup() {
    if (blocks != NULL) {
        for (int i=0; i<ROWS; i++) {
            free(blocks[i]);
        }
        free(blocks);
        blocks = NULL;
    }
}

void blockInitAll() {
    int i,j;

    blockSetDefaults();

    animating = false;
    bump_timer = 0;
    bump_pixels = 0;
    speed = speed_init;
    speed_timer = SPEED_TIME;
    game_over_timer = 0;
    jewels_cursor_select = false;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            blockSet(i,j,false,-1);
        }
    }

    if (game_mode == GAME_MODE_JEWELS) {
        do {
            for(i=ROWS-START_ROWS;i<ROWS;i++) {
                for(j=0;j<COLS;j++) {
                    blockSet(i,j,true,blockRand());
                }
            }
        } while (blockHasMatches());
    }
    else {
        for(i=ROWS-START_ROWS;i<ROWS;i++) {
            blockAddLayerRandom(i);
        }
    }
}

void blockLogic() {
    animating = blockAnimate();

    if (animating)
        return;

    blockMatch();

    if (game_mode == GAME_MODE_JEWELS) {
        blockReturn();
        blockAddFromTop();
    }
    else {
        blockRise();
    }

    blockGravity();

    if (game_mode == GAME_MODE_JEWELS && !blockHasGaps() && !blockHasSwitchMatch())
        game_over_timer = FPS * 2;
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
            blockSet(0,j,true,blockRand());
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
                    blockSwitch(i,j,i+gap_size,j, true, true);
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

    if (cursor.y1 > CURSOR_MIN_Y) cursor.y1--;
    cursor.y2 = cursor.y1;

    for (j=0;j<COLS;j++) {
        for (i=1;i<ROWS;i++) {
            blockSwitch(i,j,i-1,j, false, false);
        }
    }

    blockAddLayerRandom(ROWS-1);

    // reset bump pixels to the previous block level
    bump_pixels -= bump_pixels % BLOCK_SIZE;

    return true;
}

bool blockHasSwitchMatch() {
    // check if no moves will result in any matches
    // do this by performing every possible switch and checking for matches
    int i,j;
    bool has_switch_match = false;

    for (j=0;j<COLS;j++) {
        for (i=0;i<ROWS;i++) {
            blockSwitch(i,j,i+1,j,false,false);
            has_switch_match = has_switch_match || blockHasMatches();
            blockSwitch(i,j,i+1,j,false,false);
            blockSwitch(i,j,i,j+1,false,false);
            has_switch_match = has_switch_match || blockHasMatches();
            blockSwitch(i,j,i,j+1,false,false);
            if (has_switch_match) return true;
        }
        if (has_switch_match) return true;
    }
    return false;
}

bool blockHasGaps() {
    int i,j;

    for (j=0;j<COLS;j++)
        for (i=0;i<ROWS;i++)
            if (!blocks[i][j].alive) return true;
    return false;
}

void blockSwitchCursor() {
    if (game_mode == GAME_MODE_JEWELS) {
        if (!jewels_cursor_select) {
            jewels_cursor_select = true;
            cursor.x2 = cursor.x1;
            cursor.y2 = cursor.y1;
            return;
        }
        else {
            jewels_cursor_select = false;
        }
    }

    // don't allow switching blocks that are already moving
    if (blocks[cursor.y1][cursor.x1].moving == false && blocks[cursor.y2][cursor.x2].moving == false) {
        blockSwitch(cursor.y1, cursor.x1, cursor.y2, cursor.x2, true, false);
        if (game_mode == GAME_MODE_JEWELS) {
            blocks[cursor.y1][cursor.x1].return_row = cursor.y2;
            blocks[cursor.y1][cursor.x1].return_col = cursor.x2;
            cursor.x2 = cursor.x1;
            cursor.y2 = cursor.y1;
        }
    }
}

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

#include "game_mode.h"
#include "block.h"

static void defaultSetDefaults(void);
static void jewelsSetDefaults(void);
static void dropSetDefaults(void);
static void defaultInitAll(void);
static void jewelsInitAll(void);
static void dropInitAll(void);
static void defaultBlockLogic(void);
static void jewelsBlockLogic(void);
static void dropBlockLogic(void);
static void defaultStatusText(char *buf, int score, int speed);
static void jewelsStatusText(char *buf, int score, int speed);
static void defaultSetCursor(struct Cursor *cursor);
static void jewelsSetCursor(struct Cursor *cursor);
static void dropSetCursor(struct Cursor *cursor);
static void defaultBump(void);
static void jewelsBump(void);
static void dropBump(void);

void gameModeInit() {
    game_mode_default.setDefaults = defaultSetDefaults;
    game_mode_default.drawOffsetExtraY = BLOCK_SIZE-(surface_bar->h);
    game_mode_default.initAll = defaultInitAll;
    game_mode_default.blockLogic = defaultBlockLogic;
    game_mode_default.background = surface_background;
    game_mode_default.statusText = defaultStatusText;
    game_mode_default.music = music;
    game_mode_default.speed = true;
    game_mode_default.setCursor = defaultSetCursor;
    game_mode_default.bump = defaultBump;
    game_mode_default.highscores = &path_file_highscores;

    game_mode_jewels = game_mode_default;
    game_mode_jewels.setDefaults = jewelsSetDefaults;
    game_mode_jewels.drawOffsetExtraY = 0;
    game_mode_jewels.initAll = jewelsInitAll;
    game_mode_jewels.blockLogic = jewelsBlockLogic;
    game_mode_jewels.background = surface_background_jewels;
    game_mode_jewels.statusText = jewelsStatusText;
    game_mode_jewels.music = music_jewels;
    game_mode_jewels.speed = false;
    game_mode_jewels.setCursor = jewelsSetCursor;
    game_mode_jewels.bump = jewelsBump;
    game_mode_jewels.highscores = &path_file_highscores_jewels;

    game_mode_drop = game_mode_default;
    game_mode_drop.setDefaults = dropSetDefaults;
    game_mode_drop.drawOffsetExtraY = 0;
    game_mode_drop.initAll = dropInitAll;
    game_mode_drop.blockLogic = dropBlockLogic;
    game_mode_drop.setCursor = dropSetCursor;
    game_mode_drop.bump = dropBump;
    game_mode_drop.highscores = &path_file_highscores_drop;
}

static void defaultSetDefaults(void) {
    ROWS = 10;
    COLS = 13;
    NUM_BLOCKS = 7;
    START_ROWS = 4;
    DISABLED_ROWS = 1;
    CURSOR_MAX_X = COLS-2;
    CURSOR_MIN_Y = 1;
    BLOCK_MOVE_FRAMES = 4;
}
static void jewelsSetDefaults(void) {
    ROWS = 8;
    COLS = 8;
    NUM_BLOCKS = 7;
    START_ROWS = ROWS;
    DISABLED_ROWS = 0;
    CURSOR_MAX_X = COLS-1;
    CURSOR_MIN_Y = 0;
    BLOCK_MOVE_FRAMES = 8;
}
static void dropSetDefaults(void) {
    ROWS = 10;
    COLS = 13;
    NUM_BLOCKS = 4;
    START_ROWS = 4;
    DISABLED_ROWS = 1;
    CURSOR_MAX_X = COLS-1;
    CURSOR_MIN_Y = 1;
    BLOCK_MOVE_FRAMES = 4;
}

static void defaultInitAll(void) {
    for(int i=ROWS-START_ROWS;i<ROWS;i++) {
        blockAddLayerRandom(i);
    }
}
static void jewelsInitAll(void) {
    do {
        for(int i=ROWS-START_ROWS;i<ROWS;i++) {
            for(int j=0;j<COLS;j++) {
                blockSet(i,j,true,blockRand());
            }
        }
    } while (blockHasMatches());
}
static void dropInitAll(void) {
    for(int i=ROWS-START_ROWS;i<ROWS;i++) {
        for(int j=0;j<COLS;j++) {
            blockSet(i,j,true,blockRand());
        }
    }
}

static void defaultBlockLogic(void) {
    blockMatch();
    blockRise();
    blockGravity();
}
static void jewelsBlockLogic(void) {
    blockMatch();
    blockReturn();
    blockAddFromTop();
    blockGravity();
    if (!blockHasGaps() && !blockHasSwitchMatch())
        game_over_timer = FPS * 2;
}
static void dropBlockLogic(void) {
    blockRise();
    blockGravity();
}

static void defaultStatusText(char *buf, int score, int speed) {
    sprintf(buf, "Score: %-10d  Speed: %d", score, speed);
}
static void jewelsStatusText(char *buf, int score, int speed) {
    sprintf(buf, "Score: %-10d", score);
}

static void defaultSetCursor(struct Cursor *cursor) {
    cursor->x2 = cursor->x1 + 1;
    cursor->y2 = cursor->y1;
}
static void jewelsSetCursor(struct Cursor *cursor) {
    cursor->x2 = cursor->x1;
    cursor->y2 = cursor->y1;
}
static void dropSetCursor(struct Cursor *cursor) {
    // always set cursor to top block in column
    for (int i=ROWS-1;i>=0;i--) {
        if (!blocks[i][cursor->x1].alive) {
            cursor->y1 = min(max(i + 1, 0), ROWS - 1);
            break;
        }
    }
    cursor->x2 = cursor->x1;
    cursor->y2 = cursor->y1;
}

static void defaultBump(void) {
    if (blockAddLayer())
        score += POINTS_PER_BUMP;
}
static void jewelsBump(void) {
    jewels_cursor_select = false;
}
static void dropBump(void) {
}
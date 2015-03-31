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
static void defaultStatusText(char *buf, int _score, int _speed);
static void jewelsStatusText(char *buf, int _score, int _speed);
static void defaultSetCursor(struct Cursor *_cursor);
static void jewelsSetCursor(struct Cursor *_cursor);
static void dropSetCursor(struct Cursor *_cursor);
static void defaultSwitch(struct Cursor *_cursor);
static void jewelsSwitch(struct Cursor *_cursor);
static void dropSwitch(struct Cursor *_cursor);
static void defaultBump(struct Cursor *_cursor);
static void jewelsBump(struct Cursor *_cursor);
static void dropBump(struct Cursor *_cursor);
static void defaultGetHeld(int *color, int *amount);
static void jewelsGetHeld(int *color, int *amount);
static void dropGetHeld(int *color, int *amount);

static int dropColor = -1;
static int dropAmount = 0;

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
    game_mode_default.doSwitch = defaultSwitch;
    game_mode_default.bump = defaultBump;
    game_mode_default.getHeld= defaultGetHeld;
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
    game_mode_jewels.doSwitch = jewelsSwitch;
    game_mode_jewels.bump = jewelsBump;
    game_mode_jewels.getHeld= jewelsGetHeld;
    game_mode_jewels.highscores = &path_file_highscores_jewels;

    game_mode_drop = game_mode_default;
    game_mode_drop.setDefaults = dropSetDefaults;
    game_mode_drop.drawOffsetExtraY = BLOCK_SIZE-(surface_bar->h)+(BLOCK_SIZE/2);
    game_mode_drop.initAll = dropInitAll;
    game_mode_drop.blockLogic = dropBlockLogic;
    game_mode_drop.background = surface_background_drop;
    game_mode_drop.setCursor = dropSetCursor;
    game_mode_drop.doSwitch = dropSwitch;
    game_mode_drop.bump = dropBump;
    game_mode_drop.getHeld= dropGetHeld;
    game_mode_drop.highscores = &path_file_highscores_drop;
}

int gameModeGetIndex() {
    if (game_mode == &game_mode_default)
        return GAME_MODE_DEFAULT;
    else if (game_mode == &game_mode_jewels)
        return GAME_MODE_JEWELS;
    else if (game_mode == &game_mode_drop)
        return GAME_MODE_DROP;
    else
        return GAME_MODE_DEFAULT;
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
    ROWS = 9;
    COLS = 8;
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

    dropColor = -1;
    dropAmount = 0;
}

static void defaultBlockLogic(void) {
    blockClearMatches();
    blockFindMatch3();
    blockRise();
    blockGravity();
}
static void jewelsBlockLogic(void) {
    blockClearMatches();
    blockFindMatch3();
    blockReturn();
    blockAddFromTop();
    blockGravity();
    if (!blockHasGaps() && !blockHasSwitchMatch())
        game_over_timer = FPS * 2;
}
static void dropBlockLogic(void) {
    blockClearMatches();
    blockRise();
    blockGravity();
}

static void defaultStatusText(char *buf, int _score, int _speed) {
    sprintf(buf, "Score: %-10d  Speed: %d", _score, _speed);
}
static void jewelsStatusText(char *buf, int _score, int _speed) {
    if (_speed) {} // unused
    sprintf(buf, "Score: %-10d", _score);
}

static void defaultSetCursor(struct Cursor *_cursor) {
    _cursor->x2 = _cursor->x1 + 1;
    _cursor->y2 = _cursor->y1;
}
static void jewelsSetCursor(struct Cursor *_cursor) {
    _cursor->x2 = _cursor->x1;
    _cursor->y2 = _cursor->y1;
}
static void dropSetCursor(struct Cursor *_cursor) {
    // always set cursor to top block in column
    for (int i=CURSOR_MAX_Y;i>=0;i--) {
        if (!blocks[i][_cursor->x1].alive) {
            _cursor->y1 = min(max(i + 1, 0), CURSOR_MAX_Y);
            break;
        }
    }
    _cursor->x2 = _cursor->x1;
    _cursor->y2 = _cursor->y1;
}

static void defaultBump(struct Cursor *_cursor) {
    if (_cursor) {} // unused
    if (blockAddLayer())
        score += POINTS_PER_BUMP;
}
static void jewelsBump(struct Cursor *_cursor) {
    if (_cursor) {} // unused
    jewels_cursor_select = false;
}
static void dropBump(struct Cursor *_cursor) {
    // don't grab if no blocks in column
    if (_cursor->y1 == ROWS - 1 || !blocks[_cursor->y1][_cursor->x1].alive) {
        return;
    }
    int color = blocks[_cursor->y1][_cursor->x1].color;
    // if currently no blocks grabbed, grab any color
    if (dropColor == -1) {
        dropColor = color;
    }
    // only grab blocks of the same color
    if (color != dropColor) {
        return;
    }
    // Grab the blocks TODO: animate
    for (int i = _cursor->y1; i < ROWS-DISABLED_ROWS; i++) {
        if (blocks[i][_cursor->x1].color != color) {
            break;
        }
        blocks[i][_cursor->x1].alive = false;
        dropAmount++;
    }
    Mix_PlayChannel(-1,sound_switch,0);
}

static void defaultSwitch(struct Cursor *_cursor) {
    if (_cursor) {} // unused
    blockSwitchCursor();
}
static void jewelsSwitch(struct Cursor *_cursor) {
    if (!jewels_cursor_select) {
        jewels_cursor_select = true;
        _cursor->x2 = _cursor->x1;
        _cursor->y2 = _cursor->y1;
        return;
    }
    else {
        jewels_cursor_select = false;
    }

    if (blockSwitchCursor()) {
        blocks[_cursor->y1][_cursor->x1].return_row = _cursor->y2;
        blocks[_cursor->y1][_cursor->x1].return_col = _cursor->x2;
        _cursor->x2 = _cursor->x1;
        _cursor->y2 = _cursor->y1;
    }
}
static void dropSwitch(struct Cursor *_cursor) {
    if (dropAmount == 0) {
        return;
    }
    // Eject all the blocks held TODO: animate
    int i;
    for (i = _cursor->y1; i >= 0 && dropAmount > 0; i--, dropAmount--) {
        if (blocks[i][_cursor->x1].alive) {
            dropAmount++;
            continue;
        }
        blocks[i][_cursor->x1].color = dropColor;
        blocks[i][_cursor->x1].alive = true;
    }
    // Check if there is a match in the current column
    if (blockMatchVertical(i + 1, _cursor->x1) > 1) {
        // Perform a flooding match from the dropped blocks
        blockMatchAdjacent(i + 1, _cursor->x1);
        Mix_PlayChannel(-1,sound_match,0);
    }
    if (dropAmount == 0) {
        dropColor = -1;
    }
}

static void defaultGetHeld(int *color, int *amount) {
    // unused
    if (color || amount) {}
    return;
}

static void jewelsGetHeld(int *color, int *amount) {
    // unused
    if (color || amount) {}
    return;
}

static void dropGetHeld(int *color, int *amount) {
    if (color)
        *color = dropColor;

    if (amount)
        *amount = dropAmount;
}

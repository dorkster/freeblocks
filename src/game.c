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

#include <stdlib.h>

#include "block.h"
#include "game.h"
#include "sys.h"

void gameInit() {
    cursor_x = 3;
    cursor_y = 7;
    game_over = false;
    score = 0;

    blockInitAll();
}

void gameLogic() {
    if (action_cooldown > 0) action_cooldown--;

    if (game_over) {
        gameOver();
    } else {
        blockLogic();
        gameMove();
        gameSwitch();
        gameBump();
    }
}

void gameMove() {
    if (action_cooldown > 0) return;

    if (action_moveleft && cursor_x > 0) cursor_x--;
    else if (action_moveright && cursor_x < 6) cursor_x++;
    else if (action_moveup && cursor_y > 0) cursor_y--;
    else if (action_movedown && cursor_y < 14) cursor_y++;
    else return;

    action_cooldown = 10;
}

void gameSwitch() {
    if (action_cooldown > 0) return;

    if (action_switch) {
        blockSwitch(cursor_y, cursor_x, cursor_y, cursor_x+1);
        action_switch = false;
    }
}

void gameBump() {
    if (action_cooldown > 0) return;

    if (action_bump) {
        blockAddLayer();
        action_bump = false;
    }
}

void gameOver() {
    if (action_cooldown > 0) return;

    if (action_switch || action_bump) {
        gameInit();
    }
}

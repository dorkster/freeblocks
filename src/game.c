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

void gameTitle() {
    title_screen = true;
    game_over = false;
    score = 0;
    title_option = TITLE_PLAY;
}

void gameInit() {
    title_screen = false;
    cursor_x = (COLS/2)-1;
    cursor_y = ROWS-START_ROWS-1;

    blockInitAll();
}

void gameLogic() {
    if (action_cooldown > 0 && !paused) action_cooldown--;

    // handle the title screen input
    if (title_screen) {
        if (action_switch) {
            action_switch = false;
            if (title_option == TITLE_PLAY) gameInit();
            // TODO add high scores list here
            // else if (title_option == TITLE_HIGHSCORES)
            else if (title_option == TITLE_QUIT) quit = true;
        } else if (action_left && action_cooldown == 0 && title_option > 0) {
            title_option--;
            action_cooldown = 20;
        } else if (action_right && action_cooldown == 0 && title_option < TITLE_QUIT) {
            title_option++;
            action_cooldown = 20;
        }
        return;
    }

    if (game_over) {
        gameOver();
    } else {
        gamePause(); // check if the pause key is pressed
        if (!paused) {
            blockLogic();
            gameMove();
            gameSwitch();
            gameBump();
        }
    }
}

void gameMove() {
    if (cursor_y < 1) cursor_y = 1;
    if (action_cooldown > 0) return;

    if (action_left && cursor_x > 0) cursor_x--;
    else if (action_right && cursor_x < COLS-2) cursor_x++;
    else if (action_up && cursor_y > 1) cursor_y--;
    else if (action_down && cursor_y < ROWS-2) cursor_y++;
    else return;

    action_cooldown = 10;
}

void gameSwitch() {
    if (action_switch) {
        blockSwitch(cursor_y, cursor_x, cursor_y, cursor_x+1);
        action_switch = false;
    }
}

void gameBump() {
    if (action_bump) {
        blockAddLayer();
        action_bump = false;
    }
}

void gameOver() {
    if (action_cooldown > 0) return;

    if (action_switch) {
        action_switch = false;
        gameTitle();
    }
}

void gamePause() {
    if (action_cooldown > 0) return;

    if (action_pause) {
        if (!paused) paused = true;
        else paused = false;

        action_pause = false;
    }
}

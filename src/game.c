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
#include "menu.h"
#include "sys.h"

void gameTitle() {
    title_screen = true;
    high_scores_screen = false;
    game_over = false;
    score = 0;
    Mix_FadeOutMusic(2000);

    menuAdd("Play Game");
    menuAdd("High Scores");
    menuAdd("Quit");
}

void gameHighScores() {
    title_screen = false;
    high_scores_screen = true;

    menuAdd("Return to title screen");
}

void gameInit() {
    title_screen = false;
    score = 0;
    cursor_x = (COLS/2)-1;
    cursor_y = ROWS-START_ROWS-1;

    blockInitAll();

    Mix_VolumeMusic(MIX_MAX_VOLUME);
    if (!game_over) Mix_PlayMusic(music,-1);

    game_over = false;
}

void gameLogic() {
    int menu_choice;

    if (action_cooldown > 0) action_cooldown--;

    // handle the title screen menu
    if (title_screen) {
        menu_choice = menuLogic();
        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) gameInit();
            else if (menu_choice == 1) gameHighScores();
            else if (menu_choice == 2) quit = true;
        }
        return;
    }

    // handle high scores screen menu
    if (high_scores_screen) {
        menu_choice = menuLogic();
        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) gameTitle();
        }
        return;
    }

    // handle game over menu
    if (game_over) {
        menu_choice = menuLogic();
        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) {
                gameAddHighScore(score);
                gameInit();
            } else if (menu_choice == 1) {
                gameAddHighScore(score);
                gameTitle();
            }
        }
        return;
    }

    // handle gameplay input
    if (trigger_game_over) {
        gameOver();
    } else {
        gamePause(); // check if the pause key is pressed
        // handle pause screen menu
        if (paused) {
            menu_choice = menuLogic();
            if (menu_choice > -1) {
                menuClear();
                if (menu_choice == 0) {
                    paused = false;
                    Mix_VolumeMusic(MIX_MAX_VOLUME);
                } else if (menu_choice == 1) {
                    paused = false;
                    gameAddHighScore(score);
                    gameTitle();
                }
            }
        } else {
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

    if (action_left && cursor_x > 0) {
        cursor_x--;
        Mix_PlayChannel(-1,sound_switch,0);
    } else if (action_right && cursor_x < COLS-2) {
        cursor_x++;
        Mix_PlayChannel(-1,sound_switch,0);
    } else if (action_up && cursor_y > 1) {
        cursor_y--;
        Mix_PlayChannel(-1,sound_switch,0);
    } else if (action_down && cursor_y < ROWS-2) {
        cursor_y++;
        Mix_PlayChannel(-1,sound_switch,0);
    } else return;

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
        if (blockAddLayer()) score += POINTS_PER_BUMP;
        action_bump = false;
    }
}

void gameOver() {
    if (action_cooldown > 0) return;

    if (action_switch) {
        action_switch = false;
        gameAddHighScore(score);
        gameTitle();
    }

    trigger_game_over = false;
    game_over = true;

    menuAdd("Try again");
    menuAdd("Return to title screen");
}

void gamePause() {
    if (action_cooldown > 0) return;

    if (action_pause) {
        menuClear();
        Mix_PlayChannel(-1,sound_menu,0);

        if (!paused) {
            paused = true;
            Mix_VolumeMusic(MIX_MAX_VOLUME/2);
            menuAdd("Continue playing");
            menuAdd("Quit to title screen");
        }
        else {
            paused = false;
            Mix_VolumeMusic(MIX_MAX_VOLUME);
        }

        action_pause = false;
    }
}

void gameAddHighScore(int _score) {
    for (int i=0; i<10; i++) {
        if (_score > high_scores[i]) {
            for (int j=9; j>i; j--) {
                high_scores[j] = high_scores[j-1];
            }
            high_scores[i] = _score;
            return;
        }
    }
}


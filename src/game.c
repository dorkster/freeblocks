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
    options_screen = -1;

    game_over = false;
    score = 0;
    Mix_FadeOutMusic(2000);

    menuAdd("Play Game", 0, 0);
    menuAdd("Game Type", GAME_MODE_DEFAULT, GAME_MODE_JEWELS);
    menuAdd("Speed Level", 1, MAX_SPEED);
    menuAdd("High Scores", 0, 0);
    menuAdd("Options", 0, 0);
    menuAdd("Quit", 0, 0);

    menuItemSetOptionText(1, GAME_MODE_DEFAULT, "Normal");
    menuItemSetOptionText(1, GAME_MODE_JEWELS, "Jewels");
    menuItemSetVal(1, game_mode);
}

void gameHighScores() {
    title_screen = false;
    high_scores_screen = true;
    options_screen = -1;

    sysHighScoresLoad();

    menuAdd("Return to title screen", 0, 0);
}

void gameOptions() {
    title_screen = false;
    high_scores_screen = false;
    options_screen = OPTIONS_MAIN;

    // 0 joystick
    menuAdd("Joystick", 0, SDL_NumJoysticks());
    menuItemSetVal(0, option_joystick+1);
    menuItemSetOptionText(0, 0, "(no joystick)");
    int i;
    for (i=0; i < SDL_NumJoysticks(); i++) {
        menuItemSetOptionText(0, i+1, SDL_JoystickName(i));
    }

    // 1 sound level
    menuAdd("Sound", 0, 8);
    menuItemSetVal(1, option_sound);
    menuItemSetOptionText(1, 0, "Off");
    menuItemSetOptionText(1, 8, "Max");

    // 2 music level
    menuAdd("Music", 0, 8);
    menuItemSetVal(2, option_music);
    menuItemSetOptionText(2, 0, "Off");
    menuItemSetOptionText(2, 8, "Max");

#ifndef __GCW0__
    // 3 fullscreen toggle
    menuAdd("Fullscreen", 0, 1);
    menuItemSetVal(3, option_fullscreen);
    menuItemSetOptionText(3, 0, "Off");
    menuItemSetOptionText(3, 1, "On");
#endif

    menuAdd("Cancel", 0, 0);
    menuAdd("Save settings", 0, 0);
}

void gameInit() {
    title_screen = false;
    score = 0;
    cursor_moving = false;
    cursor_timer = -1;

    blockInitAll();
    cursor.x1 = (COLS/2)-1;
    cursor.y1 = ROWS-START_ROWS;
    if (cursor.y1 > CURSOR_MAX_Y) cursor.y1 = CURSOR_MAX_Y;


    Mix_VolumeMusic(option_music*16);
    if (!game_over)
        if (game_mode == GAME_MODE_JEWELS)
            Mix_PlayMusic(music_jewels,-1);
        else
            Mix_PlayMusic(music,-1);

    game_over = false;
}

void gameLogic() {
    int menu_choice;

    if (action_cooldown > 0) action_cooldown--;

    // handle the title screen menu
    if (title_screen) {
        if (action_exit) {
            action_exit = false;
            menu_choice = menu_size-1; // quit game
        }
        else {
            menu_choice = menuLogic();
        }

        // get the "Game Type" value
        game_mode = menuItemGetVal(1);

        if (game_mode == GAME_MODE_JEWELS)
            menuItemSetEnabled(2, false);
        else
            menuItemSetEnabled(2, true);

        if (menu_choice > -1) {
            // get the "Speed Level" value
            speed_init = menuItemGetVal(2);

            menuClear();
            if (menu_choice == 0) gameInit();
            // menu_choice == 1 game type selection
            // menu_choice == 2 speed selection
            else if (menu_choice == 3) gameHighScores();
            else if (menu_choice == 4) gameOptions();
            else if (menu_choice == 5) quit = true;
        }
        return;
    }

    // handle high scores screen menu
    if (high_scores_screen) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-1; // return to title
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) gameTitle();
        }
        return;
    }

    // handle options screen menu
    if (options_screen > -1) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-2; // exit without saving changes
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            if (menu_choice == menu_size-2) {
                menuClear();
                gameTitle();
            }
            else if (menu_choice == menu_size-1) {
                option_joystick = (int)menuItemGetVal(0)-1;
                option_sound = menuItemGetVal(1);
                option_music = menuItemGetVal(2);
#ifndef __GCW0__
                option_fullscreen = menuItemGetVal(3);
#endif

                menuClear();
                sysConfigApply();
                sysConfigSave();
                gameTitle();
            }
        }
        return;
    }

    // handle game over menu
    if (game_over) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-1; // return to title
        }
        else {
            menu_choice = menuLogic();
        }

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
    if (game_over_timer > 0) {
        gameOver();
    } else {
        gamePause(); // check if the pause key is pressed
        // handle pause screen menu
        if (paused) {
            if (action_bump) {
                action_bump = false;
                menu_choice = 0; // return to gameplay
            }
            else {
                menu_choice = menuLogic();
            }

            if (menu_choice > -1) {
                menuClear();
                if (menu_choice == 0) {
                    paused = false;
                    Mix_VolumeMusic(option_music*16);
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
    cursor_moving = false;
    if (cursor.y1 < CURSOR_MIN_Y) cursor.y1 = CURSOR_MIN_Y;
    if (action_move != action_last_move) cursor_timer = -1;
    if (action_move == action_last_move && action_cooldown > 0) return;

    if (game_mode == GAME_MODE_JEWELS && jewels_cursor_select) {
        switch (action_move) {
        case ACTION_LEFT:
            if (cursor.x1 > 0) {
                cursor.x2 = cursor.x1 - 1;
                cursor.y2 = cursor.y1;
                cursor_moving = true;
                blockSwitchCursor();
            }
            break;
        case ACTION_RIGHT:
            if (cursor.x1 < CURSOR_MAX_X) {
                cursor.x2 = cursor.x1 + 1;
                cursor.y2 = cursor.y1;
                cursor_moving = true;
                blockSwitchCursor();
            }
            break;
        case ACTION_UP:
            if (cursor.y1 > CURSOR_MIN_Y) {
                cursor.y2 = cursor.y1 - 1;
                cursor.x2 = cursor.x1;
                cursor_moving = true;
                blockSwitchCursor();
            }
            break;
        case ACTION_DOWN:
            if (cursor.y1 < CURSOR_MAX_Y) {
                cursor.y2 = cursor.y1 + 1;
                cursor.x2 = cursor.x1;
                cursor_moving = true;
                blockSwitchCursor();
            }
            break;
        case ACTION_NONE:
            break;
        }
    }
    else {
        switch (action_move) {
        case ACTION_LEFT:
            if (cursor.x1 > 0) {
                cursor.x1--;
                cursor_moving = true;
            }
            break;
        case ACTION_RIGHT:
            if (cursor.x1 < CURSOR_MAX_X) {
                cursor.x1++;
                cursor_moving = true;
            }
            break;
        case ACTION_UP:
            if (cursor.y1 > CURSOR_MIN_Y) {
                cursor.y1--;
                cursor_moving = true;
            }
            break;
        case ACTION_DOWN:
            if (cursor.y1 < CURSOR_MAX_Y) {
                cursor.y1++;
                cursor_moving = true;
            }
            break;
        case ACTION_NONE:
            break;
        }

        cursor.x2 = (game_mode == GAME_MODE_JEWELS) ? cursor.x1 : cursor.x1+1;
        cursor.y2 = cursor.y1;
    }

    if (cursor_moving) {
        Mix_PlayChannel(-1,sound_switch,0);

        if (cursor_timer == -1)
            cursor_timer = FPS/5;

        if (cursor_timer > 0)
            cursor_timer--;

        if (cursor_timer == 0)
            action_cooldown = ACTION_COOLDOWN/2;
        else
            action_cooldown = ACTION_COOLDOWN;

        action_last_move = action_move;
    }
    else {
        cursor_timer = -1;
    }
}

void gameSwitch() {
    if (action_switch) {
        blockSwitchCursor();
        action_switch = false;
    }
}

void gameBump() {
    if (action_bump) {
        if (game_mode == GAME_MODE_JEWELS) {
            jewels_cursor_select = false;
        }
        else {
            if (blockAddLayer())
                score += POINTS_PER_BUMP;
        }
        action_bump = false;
    }
}

void gameOver() {
    game_over_timer--;

    if (game_over_timer == 0) {
        game_over = true;
        menuAdd("Try again", 0, 0);
        menuAdd("Return to title screen", 0, 0);
    }
}

void gamePause() {
    if (action_cooldown > 0) return;

    if (action_pause) {
        menuClear();
        Mix_PlayChannel(-1,sound_menu,0);

        if (!paused) {
            paused = true;
            Mix_VolumeMusic(option_music*8);
            menuAdd("Continue playing", 0, 0);
            menuAdd("Quit to title screen", 0, 0);
        }
        else {
            paused = false;
            Mix_VolumeMusic(option_music*16);

            // to prevent "pre-loading" directional input while paused
            cursor_timer = -1;
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
            sysHighScoresSave();
            return;
        }
    }
}


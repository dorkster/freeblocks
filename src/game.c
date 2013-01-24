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

    menuAdd("Play Game");
    menuAdd("High Scores");
    menuAdd("Options");
    menuAdd("Quit");
}

void gameHighScores() {
    title_screen = false;
    high_scores_screen = true;
    options_screen = -1;

    menuAdd("Return to title screen");
}

void gameOptions() {
    title_screen = false;
    high_scores_screen = false;
    options_screen = OPTIONS_MAIN;

    menuAdd("Joystick");
    menuAdd("Sound");
    menuAdd("Music");

    if (option_fullscreen == 0) menuAdd("Fullscreen: Off");
    else menuAdd("Fullscreen: On");

    menuAdd("Return to title screen");
}

void gameInit() {
    title_screen = false;
    score = 0;
    cursor_x = (COLS/2)-1;
    cursor_y = ROWS-START_ROWS-1;

    blockInitAll();

    Mix_VolumeMusic(option_music*16);
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
            else if (menu_choice == 2) gameOptions();
            else if (menu_choice == 3) quit = true;
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

    // handle options screen menu
    if (options_screen > -1) {
        menu_choice = menuLogic();
        if (menu_choice > -1) {
            if (options_screen == OPTIONS_JOYSTICK) {
                option_joystick = menu_option-1;
                sysConfigApply();
                menuClear();
                gameOptions();
            } else if (options_screen == OPTIONS_SOUND) {
                option_sound = menu_option;
                sysConfigApply();
                menuClear();
                gameOptions();
            } else if (options_screen == OPTIONS_MUSIC) {
                option_music = menu_option;
                sysConfigApply();
                menuClear();
                gameOptions();
            } else {
                if (menu_choice == 0) {
                    options_screen = OPTIONS_JOYSTICK;
                    menuClear();
                    menuAdd("(No joystick)");
                    int i=0;
                    while (i < SDL_NumJoysticks() && i < MAX_MENU_ITEMS) {
                        menuAdd(SDL_JoystickName(i));
                        i++;
                    }
                    if (option_joystick+1 < menu_size) menu_option = option_joystick+1;
                } else if (menu_choice == 1) {
                    options_screen = OPTIONS_SOUND;
                    menuClear();
                    int i=0;
                    while (i < 9) {
                        if (i == 0) {
                            menuAdd("Sound off");
                        } else {
                            char *temp = malloc(strlen("Sound level: ")+2);
                            if (temp) {
                                sprintf(temp,"Sound level: %-1d",i);
                                menuAdd(temp);
                                free(temp);
                            }
                        }
                        i++;
                    }
                    if (option_sound < menu_size) menu_option = option_sound;
                } else if (menu_choice == 2) {
                    options_screen = OPTIONS_MUSIC;
                    menuClear();
                    int i=0;
                    while (i < 9) {
                        if (i == 0) {
                            menuAdd("Music off");
                        } else {
                            char *temp = malloc(strlen("Music level: ")+2);
                            if (temp) {
                                sprintf(temp,"Music level: %-1d",i);
                                menuAdd(temp);
                                free(temp);
                            }
                        }
                        i++;
                    }
                    if (option_music < menu_size) menu_option = option_music;
                } else if (menu_choice == 3) {
                    if (option_fullscreen == 0) {
                        option_fullscreen = 1;
                        menuUpdate(3,"Fullscreen: On");
                    } else {
                        option_fullscreen = 0;
                        menuUpdate(3,"Fullscreen: Off");
                    }
                    sysConfigApply();
                } else if (menu_choice == 4) {
                    menuClear();
                    sysConfigSave();
                    gameTitle();
                }
            }
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

    action_cooldown = ACTION_COOLDOWN;
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
            Mix_VolumeMusic(option_music*8);
            menuAdd("Continue playing");
            menuAdd("Quit to title screen");
        }
        else {
            paused = false;
            Mix_VolumeMusic(option_music*16);
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


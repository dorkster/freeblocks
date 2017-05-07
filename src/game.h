/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2012-2017 Justin Jacobs

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

#ifndef GAME_H
#define GAME_H

#include "sys.h"

bool cursor_moving;
int cursor_timer;
int rebind_index;

void gameTitle();
void gameHighScores();
void gameOptions();
void gameOptionsControls();
void gameOptionsRebind();
void gameInit();
void gameLogic();
void gameMove();
void gameSwitch();
void gameBump();
void gamePickUp();
void gameOver();
void gamePause();
void gameAddHighScore(int _score);

#endif

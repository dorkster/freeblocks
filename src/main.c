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

#include <SDL.h>
#include <stdlib.h>
#include <time.h>

#include "block.h"
#include "draw.h"
#include "game.h"
#include "game_mode.h"
#include "menu.h"
#include "sys.h"

int main(int argc, char *argv[]) {
    srand(time(0));

    if(!sysInit()) return 1;
    if(!sysLoadFiles()) return 1;

    gameModeInit();
    menuInit();
    gameTitle();

    while(!quit) {
        startTimer = SDL_GetTicks();

        sysInput();
        gameLogic();
        drawEverything();

        // Update the screen
        SDL_RenderPresent(renderer);
        
        // Limit the frame rate
        endTimer = SDL_GetTicks();
        deltaTimer = endTimer - startTimer;
        if(deltaTimer < (1000/FPS))
            SDL_Delay((1000/FPS)-deltaTimer);
    }
    blockCleanup();
    sysCleanup();
}

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

#include "SDL/SDL_ttf.h"
#include "SDL/SDL_gfxPrimitives.h"

#include "block.h"
#include "draw.h"
#include "sys.h"

void drawEverything() {
    // Fill the screen with black
    SDL_FillRect(screen,NULL, 0x1b1b1b);
    
    drawBlocks();
    drawCursor();
    drawInfo();
}

void drawCursor() {
    SDL_Rect dest;
    dest.x = cursor_x*BLOCK_SIZE;
    dest.y = cursor_y*BLOCK_SIZE;
    SDL_BlitSurface(surface_cursor,NULL,screen,&dest);
}

void drawBlocks() {
    int i,j;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            if(blocks[i][j].alive) {
                SDL_Rect src,dest;

                if (blocks[i][j].matched) src.x = 6 * BLOCK_SIZE;
                else src.x = blocks[i][j].color * BLOCK_SIZE;
                src.w = src.h = BLOCK_SIZE;

                dest.x = blocks[i][j].x;
                dest.y = blocks[i][j].y;

                SDL_BlitSurface(surface_blocks,&src,screen,&dest);
            }
        }
    }
}

void drawInfo() {
    char info[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;
    dest.x = 8;
    dest.y = 488;

    if (game_over) sprintf(info,"Score: %-5d  Game Over!",score);
    else sprintf(info,"Score: %-5d",score);

    text_info = TTF_RenderText_Solid(font,info,color);
    if(!text_info) return;

    SDL_BlitSurface(text_info,NULL,screen,&dest);
    SDL_FreeSurface(text_info);
}


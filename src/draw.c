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

#include "block.h"
#include "draw.h"
#include "sys.h"

void drawEverything() {
    // Fill the screen with black
    SDL_FillRect(screen,NULL, 0x000000);

    SDL_BlitSurface(surface_background,NULL,screen,NULL);

    drawBlocks();
    drawCursor();
    drawInfo();
}

void drawCursor() {
    SDL_Rect dest;
    dest.x = cursor_x*BLOCK_SIZE;
    dest.y = (cursor_y*BLOCK_SIZE) - bump_pixels;
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

                if (i == ROWS-1) src.y = BLOCK_SIZE;
                else src.y = 0;

                src.w = src.h = BLOCK_SIZE;

                dest.x = blocks[i][j].x;
                dest.y = blocks[i][j].y - bump_pixels;

                SDL_BlitSurface(surface_blocks,&src,screen,&dest);
            }
        }
    }
}

void drawInfo() {
    char info[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_statusbar->h;
    SDL_BlitSurface(surface_statusbar,NULL,screen,&dest);

    // statusbar text
    dest.x = 8;
    dest.y = SCREEN_HEIGHT-32;

    if (game_over) sprintf(info,"Score: %-5d  Game Over!",score);
    else sprintf(info,"Score: %-5d  Speed: %d",score,speed);

    text_info = TTF_RenderText_Blended(font,info,color);
    if(!text_info) return;

    SDL_BlitSurface(text_info,NULL,screen,&dest);
    SDL_FreeSurface(text_info);
}


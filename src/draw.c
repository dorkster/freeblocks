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

#include <SDL_ttf.h>

#include "block.h"
#include "draw.h"
#include "menu.h"
#include "sys.h"

void drawEverything() {
    // Fill the screen with black
    SDL_FillRect(screen,NULL, 0x000000);

    SDL_BlitSurface(surface_background,NULL,screen,NULL);

    if (title_screen) {
        drawTitle();
    } else if (high_scores_screen) {
        drawHighScores();
    } else if (options_screen > -1) {
        drawOptions();
    } else {
        drawBlocks();
        drawCursor();
        drawInfo();
    }
}

void drawMenu(int offset) {
    SDL_Surface *text;
    SDL_Color color = {217,217,217,255};
    SDL_Rect dest;
    int i;

    for (i=0;i<menu_size;i++) {
        dest.x = 0;
        dest.y = SCREEN_HEIGHT - ((menu_size-i) * surface_bar->h) - offset;
        if (i == menu_option) SDL_BlitSurface(surface_bar,NULL,screen,&dest);
        else SDL_BlitSurface(surface_bar_inactive,NULL,screen,&dest);

        text = TTF_RenderText_Blended(font, menuItemGetText(i), color);
        if (text) {
            dest.x = SCREEN_WIDTH/2 - text->w/2;
            dest.y = SCREEN_HEIGHT - ((menu_size-i-1) * surface_bar->h) - surface_bar->h - offset;
            SDL_BlitSurface(text,NULL,screen,&dest);
            SDL_FreeSurface(text);
        }
    }
}

void drawCursor() {
    // don't show the cursor when paused
    if (paused) return;

    SDL_Rect dest;
    dest.x = cursor_x*BLOCK_SIZE + DRAW_OFFSET_X;
    dest.y = (cursor_y*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
    SDL_BlitSurface(surface_cursor,NULL,screen,&dest);
}

void drawBlocks() {
    // don't show the blocks when paused
    if (paused) return;

    int i,j;

    for(i=0;i<ROWS;i++) {
        for(j=0;j<COLS;j++) {
            if(blocks[i][j].alive) {
                SDL_Rect src,dest;

                dest.x = blocks[i][j].x + DRAW_OFFSET_X;
                dest.y = blocks[i][j].y - bump_pixels + DRAW_OFFSET_Y;

                if (blocks[i][j].matched) {
                    src.x = blocks[i][j].frame * BLOCK_SIZE;
                    src.y = 0;

                    src.w = src.h = BLOCK_SIZE;

                    SDL_BlitSurface(surface_clear,&src,screen,&dest);
                } else {
                    src.x = blocks[i][j].color * BLOCK_SIZE;

                    if (i > CURSOR_MAX_Y || game_over || game_over_timer > 0) src.y = BLOCK_SIZE;
                    else src.y = 0;

                    src.w = src.h = BLOCK_SIZE;

                    SDL_BlitSurface(surface_blocks,&src,screen,&dest);
                }
            }
        }
    }
}

void drawInfo() {
    SDL_Surface *text_info;
    char text[256];
    SDL_Color color = {217,217,217,255};
    SDL_Rect dest;

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_bar->h;
    if (paused || game_over || game_over_timer > 0) SDL_BlitSurface(surface_bar_inactive,NULL,screen,&dest);
    else SDL_BlitSurface(surface_bar,NULL,screen,&dest);

    // statusbar text
    if (game_over || game_over_timer > 0) sprintf(text,"Score: %-10d  Game Over!",score);
    else {
        if (paused) sprintf(text,"Score: %-10d  *Paused*",score);
        else sprintf(text,"Score: %-10d  Speed: %d",score,speed);
    }

    text_info = TTF_RenderText_Blended(font,text,color);
    if(text_info) {
        dest.x = surface_bar->h / 4;
        dest.y = SCREEN_HEIGHT-surface_bar->h;

        SDL_BlitSurface(text_info,NULL,screen,&dest);
        SDL_FreeSurface(text_info);
    }

    // menu
    if (paused || game_over) drawMenu(surface_bar->h);
}

void drawTitle() {
    SDL_Rect dest;

    // title logo
    dest.x = 0;
    dest.y = SCREEN_HEIGHT/2 - surface_title->h/2 - surface_bar->h*2;
    SDL_BlitSurface(surface_title,NULL,screen,&dest);

    // menu
    drawMenu(0);
}

void drawHighScores() {
    SDL_Surface *text_header;
    SDL_Surface *text_score[10];
    char text[256];
    SDL_Color color = {217,217,217,255};
    SDL_Rect dest;

    // list background
    dest.x = SCREEN_WIDTH/2 - surface_highscores->w/2;
    dest.y = 0;
    SDL_BlitSurface(surface_highscores,NULL,screen,&dest);

    // "High Scores" text
    sprintf(text,"High Scores");
    text_header = TTF_RenderText_Blended(font,text,color);
    if (text_header) {
        dest.x = SCREEN_WIDTH/2 - text_header->w/2;
        dest.y = surface_bar->h/4;

        SDL_BlitSurface(text_header,NULL,screen,&dest);
        SDL_FreeSurface(text_header);
    }

    // high score list
    for (int i=0; i<10; i++) {
        if (high_scores[i] > 0) sprintf(text,"%d. %d",i+1,high_scores[i]);
        else sprintf(text,"%d.",i+1);
        text_score[i] = TTF_RenderText_Blended(font,text,color);
        if (text_score[i]) {
            dest.x = surface_highscores->w;
            dest.y = (surface_bar->h*i) + surface_bar->h*2;

            SDL_BlitSurface(text_score[i],NULL,screen,&dest);
            SDL_FreeSurface(text_score[i]);
        }
    }

    // "Return to title" text
    drawMenu(0);
}

void drawOptions() {
    // menu
    drawMenu(0);
}


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

    if (title_screen) {
        drawTitle();
    } else if (high_scores_screen) {
        drawHighScores();
    } else {
        drawBlocks();
        drawCursor();
        drawInfo();
    }
}

void drawCursor() {
    // don't show the cursor when paused
    if (paused) return;

    SDL_Rect dest;
    dest.x = cursor_x*BLOCK_SIZE;
    dest.y = (cursor_y*BLOCK_SIZE) - bump_pixels;
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
    SDL_Surface *text_info;
    char text[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_statusbar->h;
    SDL_BlitSurface(surface_statusbar,NULL,screen,&dest);

    // statusbar text
    if (game_over) sprintf(text,"Score: %-10d  Game Over!",score);
    else {
        if (paused) sprintf(text,"Score: %-10d  *Paused*",score);
        else sprintf(text,"Score: %-10d  Speed: %d",score,speed);
    }

    text_info = TTF_RenderText_Blended(font,text,color);
    if(text_info) {
        dest.x = 8;
        dest.y = SCREEN_HEIGHT-32;

        SDL_BlitSurface(text_info,NULL,screen,&dest);
        SDL_FreeSurface(text_info);
    }
}

void drawTitle() {
    SDL_Surface *text_title;
    char text[256];
    SDL_Color color = {217,217,217};
    SDL_Rect dest;

    // title logo
    dest.x = 0;
    dest.y = SCREEN_HEIGHT/2 - surface_title->h/2;
    SDL_BlitSurface(surface_title,NULL,screen,&dest);

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_statusbar->h;
    SDL_BlitSurface(surface_statusbar,NULL,screen,&dest);

    if (title_option == TITLE_PLAY) sprintf(text,"Play Game  >");
    else if (title_option == TITLE_HIGHSCORES) sprintf(text,"<  High Scores  >");
    else if (title_option == TITLE_QUIT) sprintf(text,"<  Quit Game");

    text_title = TTF_RenderText_Blended(font,text,color);
    if (text_title) {
        dest.x = SCREEN_WIDTH/2 - text_title->w/2;
        dest.y = SCREEN_HEIGHT-32;

        SDL_BlitSurface(text_title,NULL,screen,&dest);
        SDL_FreeSurface(text_title);
    }
}

void drawHighScores() {
    SDL_Surface *text_header;
    SDL_Surface *text_message;
    SDL_Surface *text_score[10];
    char text[256];
    SDL_Color color = {217,217,217};
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
        dest.y = 8;

        SDL_BlitSurface(text_header,NULL,screen,&dest);
        SDL_FreeSurface(text_header);
    }

    // high score list
    for (int i=0; i<10; i++) {
        if (high_scores[i] > 0) sprintf(text,"%d. %d",i+1,high_scores[i]);
        else sprintf(text,"%d.",i+1);
        text_score[i] = TTF_RenderText_Blended(font,text,color);
        if (text_score[i]) {
            dest.x = 240;
            dest.y = (32*i) + 64;

            SDL_BlitSurface(text_score[i],NULL,screen,&dest);
            SDL_FreeSurface(text_score[i]);
        }
    }

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - surface_statusbar->h;
    SDL_BlitSurface(surface_statusbar,NULL,screen,&dest);

    // statusbar text
    sprintf(text,"Press 'z' to return to the title screen...");
    text_message = TTF_RenderText_Blended(font,text,color);
    if (text_message) {
        dest.x = 8;
        dest.y = SCREEN_HEIGHT-32;

        SDL_BlitSurface(text_message,NULL,screen,&dest);
        SDL_FreeSurface(text_message);
    }

}

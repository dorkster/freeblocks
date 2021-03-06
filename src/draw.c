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

#include <SDL_ttf.h>

#include "block.h"
#include "draw.h"
#include "game_mode.h"
#include "menu.h"
#include "sys.h"

void drawEverything() {
    // Fill the screen with black
    SDL_RenderClear(renderer);

    sysRenderImage(game_mode->background, NULL, NULL);

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
    Image *text;
    SDL_Color color = {217,217,217,255};
    SDL_Color color_disabled = {127,127,127,255};
    SDL_Rect dest;

    for (int i=0;i<menu_size;i++) {
        dest.x = 0;
        dest.y = SCREEN_HEIGHT - ((menu_size-i) * img_bar->h) - offset;

        if (i == menu_option) {
            sysRenderImage(img_bar, NULL, &dest);
            if (menuItemHasLeftButton(i)) {
                sysRenderImage(img_bar_left, NULL, &dest);
            }
            if (menuItemHasRightButton(i)) {
                dest.x = SCREEN_WIDTH - img_bar_right->w;
                sysRenderImage(img_bar_right, NULL, &dest);
            }
        }
        else sysRenderImage(img_bar_inactive, NULL, &dest);

        if (menuItemIsEnabled(i))
            text = createText(menuItemGetText(i), &color);
        else
            text = createText(menuItemGetText(i), &color_disabled);

        if (text) {
            dest.x = SCREEN_WIDTH/2 - text->w/2;
            dest.y = SCREEN_HEIGHT - ((menu_size-i-1) * img_bar->h) - img_bar->h - offset;
            sysRenderImage(text, NULL, &dest);
            sysDestroyImage(&text);
        }
    }
}

void drawCursor() {
    // don't show the cursor when paused
    if (paused) return;

    SDL_Rect dest;
    dest.x = cursor.x1*BLOCK_SIZE + DRAW_OFFSET_X;
    dest.y = (cursor.y1*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;

    sysRenderImage(img_cursor, NULL, &dest);

    if (game_mode == &game_mode_jewels && jewels_cursor_select) {
        if (cursor.x1 > 0) {
            dest.x = (cursor.x1-1)*BLOCK_SIZE + DRAW_OFFSET_X;
            dest.y = (cursor.y1*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
            sysRenderImage(img_cursor_highlight, NULL, &dest);
        }
        if (cursor.x1 < CURSOR_MAX_X) {
            dest.x = (cursor.x1+1)*BLOCK_SIZE + DRAW_OFFSET_X;
            dest.y = (cursor.y1*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
            sysRenderImage(img_cursor_highlight, NULL, &dest);
        }
        if (cursor.y1 > CURSOR_MIN_Y) {
            dest.x = cursor.x1*BLOCK_SIZE + DRAW_OFFSET_X;
            dest.y = ((cursor.y1-1)*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
            sysRenderImage(img_cursor_highlight, NULL, &dest);
        }
        if (cursor.y1 < CURSOR_MAX_Y) {
            dest.x = cursor.x1*BLOCK_SIZE + DRAW_OFFSET_X;
            dest.y = ((cursor.y1+1)*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
            sysRenderImage(img_cursor_highlight, NULL, &dest);
        }
    }

    if (!(cursor.x1 == cursor.x2 && cursor.y1 == cursor.y2)) {
        dest.x = cursor.x2*BLOCK_SIZE + DRAW_OFFSET_X;
        dest.y = (cursor.y2*BLOCK_SIZE) - bump_pixels + DRAW_OFFSET_Y;
        sysRenderImage(img_cursor, NULL, &dest);
    }

    if (game_mode == &game_mode_drop) {
        int drop_color, drop_amount;
        game_mode->getHeld(&drop_color, &drop_amount);

        if (drop_color != -1) {
            SDL_Rect src;

            dest.x = cursor.x1 * BLOCK_SIZE + DRAW_OFFSET_X;
            dest.y = DRAW_OFFSET_Y - BLOCK_SIZE;

            src.x = drop_color * BLOCK_SIZE;
            src.y = 0;
            src.w = src.h = BLOCK_SIZE;

            sysRenderImage(img_blocks, &src, &dest);

            if (drop_amount > 1) {
                Image *text;
                SDL_Color color = {63,63,63,255};
                char amount_str[3];
                sprintf(amount_str, "%d", drop_amount);

                text = createText(amount_str, &color);
                if(text) {
                    dest.x = (cursor.x1*BLOCK_SIZE) + DRAW_OFFSET_X + (BLOCK_SIZE/2) - (text->w/2);
                    dest.y = DRAW_OFFSET_Y - BLOCK_SIZE + (BLOCK_SIZE/2) - (text->h/2);
                    sysRenderImage(text, NULL, &dest);
                    sysDestroyImage(&text);
                    text = NULL;
                }
            }
        }
    }
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

                    sysRenderImage(img_clear, &src, &dest);
                } else {
                    src.x = blocks[i][j].color * BLOCK_SIZE;

                    if (i > ROWS-1-DISABLED_ROWS || game_over || game_over_timer > 0) src.y = BLOCK_SIZE;
                    else src.y = 0;

                    src.w = src.h = BLOCK_SIZE;

                    sysRenderImage(img_blocks, &src, &dest);
                }
            }
        }
    }
}

void drawInfo() {
    Image *text_info;
    char text[256];
    SDL_Color color = {217,217,217,255};
    SDL_Rect dest;

    // statusbar background
    dest.x = 0;
    dest.y = SCREEN_HEIGHT - img_bar->h;
    if (paused || game_over || game_over_timer > 0) sysRenderImage(img_bar_inactive, NULL, &dest);
    else sysRenderImage(img_bar, NULL, &dest);

    // statusbar text
    if (game_over || game_over_timer > 0) sprintf(text,"Score: %-10d  Game Over!",score);
    else {
        if (paused) sprintf(text,"Score: %-10d  *Paused*",score);
        else {
            game_mode->statusText(text, score, speed);
        }
    }

    text_info = createText(text, &color);
    if(text_info) {
        dest.x = img_bar->h / 4;
        dest.y = SCREEN_HEIGHT-img_bar->h;

        sysRenderImage(text_info, NULL, &dest);
        sysDestroyImage(&text_info);
        text_info = NULL;
    }

    // menu
    if (paused || game_over) drawMenu(img_bar->h);
}

void drawTitle() {
    SDL_Rect dest;

    // title logo
    dest.x = 0;
    dest.y = 0;
    sysRenderImage(img_title, NULL, &dest);

    // menu
    drawMenu(0);
}

void drawHighScores() {
    Image *text_header;
    Image *text_score[10];
    char text[256];
    SDL_Color color = {217,217,217,255};
    SDL_Rect dest;

    // list background
    dest.x = SCREEN_WIDTH/2 - img_highscores->w/2;
    dest.y = 0;
    sysRenderImage(img_highscores, NULL, &dest);

    // "High Scores" text
    sprintf(text,"High Scores");
    text_header = createText(text, &color);
    if (text_header) {
        dest.x = SCREEN_WIDTH/2 - text_header->w/2;
        dest.y = img_bar->h/4;

        sysRenderImage(text_header, NULL, &dest);
        sysDestroyImage(&text_header);
        text_header = NULL;
    }

    // high score list
    for (int i=0; i<10; i++) {
        if (high_scores[i] > 0) sprintf(text,"%d. %d",i+1,high_scores[i]);
        else sprintf(text,"%d.",i+1);
        text_score[i] = createText(text, &color);
        if (text_score[i]) {
            dest.x = img_highscores->w;
            dest.y = (img_bar->h*i) + img_bar->h*2;

            sysRenderImage(text_score[i], NULL, &dest);
            sysDestroyImage(&text_score[i]);
            text_score[i] = NULL;
        }
    }

    // "Return to title" text
    drawMenu(0);
}

void drawOptions() {
    // menu
    drawMenu(0);
}

Image* createText(const char* text, const SDL_Color* color) {
    if (!text || !color) return NULL;

    Image* img = malloc(sizeof(Image));
    if (!img)
        return NULL;
    else {
        img->texture = NULL;
        img->w = 0;
        img->h = 0;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, *color);
    if (surface) {
        img->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_QueryTexture(img->texture, NULL, NULL, &(img->w), &(img->h));
        return img;
    }
    else {
        sysDestroyImage(&img);
    }
    return NULL;
}

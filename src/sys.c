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

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "sys.h"

SDL_Surface* screen = NULL;
TTF_Font* font = NULL;
SDL_Surface* surface_blocks = NULL;
SDL_Surface* surface_clear = NULL;
SDL_Surface* surface_cursor = NULL;
SDL_Surface* surface_statusbar = NULL;
SDL_Surface* surface_background = NULL;
SDL_Surface* surface_title = NULL;
SDL_Surface* surface_highscores = NULL;
Mix_Music* music = NULL;
Mix_Chunk* sound_menu = NULL;
Mix_Chunk* sound_switch = NULL;
Mix_Chunk* sound_match = NULL;

int score = 0;
bool high_scores_screen = false;
bool game_over = false;
bool paused = false;
bool quit = false;
int cursor_x = 3;
int cursor_y = 7;
bool title_screen = true;
short title_option = TITLE_PLAY;

int action_cooldown = 0;
bool action_left = false;
bool action_right = false;
bool action_up = false;
bool action_down = false;
bool action_switch = false;
bool action_bump = false;
bool action_pause = false;

bool sysInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
    
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);

    if(screen == NULL) return false;
    
    if(TTF_Init() == -1) return false;

    if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1 ) return false;
    
    SDL_WM_SetCaption("FreeBlocks",NULL);

    return true;
}

bool sysLoadFiles() {
    // font
    font = TTF_OpenFont("./res/fonts/Alegreya-Regular.ttf",20);
    if(!font) return false;
    else TTF_SetFontHinting(font, TTF_HINTING_LIGHT);

    // graphics
    surface_blocks = IMG_Load("./res/graphics/blocks.png");
    if (!surface_blocks) return false;
    else {
        SDL_Surface *cleanup = surface_blocks;
        surface_blocks = SDL_DisplayFormatAlpha(surface_blocks);
        SDL_FreeSurface(cleanup);
    }

    surface_clear = IMG_Load("./res/graphics/clear.png");
    if (!surface_clear) return false;
    else {
        SDL_Surface *cleanup = surface_clear;
        surface_clear = SDL_DisplayFormatAlpha(surface_clear);
        SDL_FreeSurface(cleanup);
    }

    surface_cursor = IMG_Load("./res/graphics/cursor.png");
    if (!surface_cursor) return false;
    else {
        SDL_Surface *cleanup = surface_blocks;
        surface_blocks = SDL_DisplayFormatAlpha(surface_blocks);
        SDL_FreeSurface(cleanup);
    }

    surface_statusbar = IMG_Load("./res/graphics/statusbar.png");
    if (!surface_statusbar) return false;
    else {
        SDL_Surface *cleanup = surface_statusbar;
        surface_statusbar = SDL_DisplayFormatAlpha(surface_statusbar);
        SDL_FreeSurface(cleanup);
    }

    surface_background = IMG_Load("./res/graphics/background.png");
    if (!surface_background) return false;
    else {
        SDL_Surface *cleanup = surface_background;
        surface_background = SDL_DisplayFormatAlpha(surface_background);
        SDL_FreeSurface(cleanup);
    }

    surface_title = IMG_Load("./res/graphics/title.png");
    if (!surface_title) return false;
    else {
        SDL_Surface *cleanup = surface_title;
        surface_title = SDL_DisplayFormatAlpha(surface_title);
        SDL_FreeSurface(cleanup);
    }

    surface_highscores = IMG_Load("./res/graphics/highscores.png");
    if (!surface_highscores) return false;
    else {
        SDL_Surface *cleanup = surface_highscores;
        surface_highscores = SDL_DisplayFormatAlpha(surface_highscores);
        SDL_FreeSurface(cleanup);
    }

    // background music
    music = Mix_LoadMUS("res/sounds/music.ogg");
    if (!music) return false;

    // sound effects
    sound_menu = Mix_LoadWAV("res/sounds/menu.wav");
    if (!sound_menu) return false;

    sound_switch = Mix_LoadWAV("res/sounds/switch.wav");
    if (!sound_switch) return false;

    sound_match = Mix_LoadWAV("res/sounds/match.wav");
    if (!sound_match) return false;

    return true;
}

void sysCleanup() {
    TTF_CloseFont(font);
    SDL_FreeSurface(surface_blocks);
    SDL_FreeSurface(surface_clear);
    SDL_FreeSurface(surface_cursor);
    SDL_FreeSurface(surface_statusbar);
    SDL_FreeSurface(surface_background);
    SDL_FreeSurface(surface_title);
    SDL_FreeSurface(surface_highscores);
    Mix_FreeMusic(music);
    Mix_FreeChunk(sound_menu);
    Mix_FreeChunk(sound_switch);
    Mix_FreeChunk(sound_match);
    SDL_Quit();
}

void sysInput() {
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_LEFT)
                action_left = true;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_right = true;
            if(event.key.keysym.sym == SDLK_UP)
                action_up = true;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_down = true;
            if(event.key.keysym.sym == 'z')
                action_switch = true;
            if(event.key.keysym.sym == 'x')
                action_bump = true;
            if(event.key.keysym.sym == 'p')
                action_pause = true;

            if(event.key.keysym.sym == SDLK_ESCAPE)
                quit = true;
        }

        if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_LEFT)
                action_left = false;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_right = false;
            if(event.key.keysym.sym == SDLK_UP)
                action_up = false;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_down = false;
            if(event.key.keysym.sym == 'z')
                action_switch = false;
            if(event.key.keysym.sym == 'x')
                action_bump = false;
            if(event.key.keysym.sym == 'p')
                action_pause = false;
        }
            
        if(event.type == SDL_QUIT) {
            quit = true;
        }
    }
}

void sysClearHighScores() {
    for (int i=0; i<10; i++) {
        high_scores[i] = 0;
    }
}

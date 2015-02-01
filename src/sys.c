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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys/stat.h"
#include "sys/types.h"

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "sys.h"

#ifdef __GCW0__
#define KEY_SWITCH SDLK_LCTRL
#define KEY_BUMP SDLK_LALT
#define SDL_FLAGS (SDL_HWSURFACE|SDL_TRIPLEBUF)
#else
#define KEY_SWITCH 'z'
#define KEY_BUMP 'x'
#define SDL_FLAGS (SDL_HWSURFACE)
#endif

SDL_Surface* screen = NULL;
TTF_Font* font = NULL;
SDL_Surface* surface_blocks = NULL;
SDL_Surface* surface_clear = NULL;
SDL_Surface* surface_cursor = NULL;
SDL_Surface* surface_bar = NULL;
SDL_Surface* surface_bar_inactive = NULL;
SDL_Surface* surface_background = NULL;
SDL_Surface* surface_title = NULL;
SDL_Surface* surface_highscores = NULL;
Mix_Music* music = NULL;
Mix_Chunk* sound_menu = NULL;
Mix_Chunk* sound_switch = NULL;
Mix_Chunk* sound_match = NULL;
SDL_Joystick* joy = NULL;

int score = 0;
bool title_screen = true;
bool high_scores_screen = false;
int options_screen = -1;
bool game_over = false;
bool paused = false;
bool quit = false;
int cursor_x = 3;
int cursor_y = 7;

int action_cooldown = 0;
bool action_left = false;
bool action_right = false;
bool action_up = false;
bool action_down = false;
bool action_switch = false;
bool action_bump = false;
bool action_pause = false;

char* config_folder = NULL;
int option_joystick = -1;
int option_sound = 8;
int option_music = 8;
int option_fullscreen =
#ifdef __GCW0__
    1;
#else
    0;
#endif

bool sysInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
   
    const int sdl_flags =
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_FLAGS);

    if(screen == NULL) return false;
    
    if(TTF_Init() == -1) return false;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) return false;
    
    SDL_WM_SetCaption("FreeBlocks",NULL);

    sysConfigSetFolder();
    sysConfigLoad();

    sysHighScoresClear();
    sysHighScoresLoad();

    return true;
}

bool sysLoadFiles() {
    // font
    font = TTF_OpenFont(PKGDATADIR "/fonts/Alegreya-Regular.ttf",20);
    if(!font) return false;
    else TTF_SetFontHinting(font, TTF_HINTING_LIGHT);

    // graphics
    surface_blocks = IMG_Load(PKGDATADIR "/graphics/blocks.png");
    if (!surface_blocks) return false;
    else {
        SDL_Surface *cleanup = surface_blocks;
        surface_blocks = SDL_DisplayFormatAlpha(surface_blocks);
        SDL_FreeSurface(cleanup);
    }

    surface_clear = IMG_Load(PKGDATADIR "/graphics/clear.png");
    if (!surface_clear) return false;
    else {
        SDL_Surface *cleanup = surface_clear;
        surface_clear = SDL_DisplayFormatAlpha(surface_clear);
        SDL_FreeSurface(cleanup);
    }

    surface_cursor = IMG_Load(PKGDATADIR "/graphics/cursor.png");
    if (!surface_cursor) return false;
    else {
        SDL_Surface *cleanup = surface_blocks;
        surface_blocks = SDL_DisplayFormatAlpha(surface_blocks);
        SDL_FreeSurface(cleanup);
    }

    surface_bar = IMG_Load(PKGDATADIR "/graphics/bar.png");
    if (!surface_bar) return false;
    else {
        SDL_Surface *cleanup = surface_bar;
        surface_bar = SDL_DisplayFormatAlpha(surface_bar);
        SDL_FreeSurface(cleanup);
    }

    surface_bar_inactive = IMG_Load(PKGDATADIR "/graphics/bar_inactive.png");
    if (!surface_bar_inactive) return false;
    else {
        SDL_Surface *cleanup = surface_bar_inactive;
        surface_bar_inactive = SDL_DisplayFormatAlpha(surface_bar_inactive);
        SDL_FreeSurface(cleanup);
    }

    surface_background = IMG_Load(PKGDATADIR
#ifdef __GCW0__
        "/graphics/background_320x240.png");
#else
        "/graphics/background.png");
#endif
    if (!surface_background) return false;
    else {
        SDL_Surface *cleanup = surface_background;
        surface_background = SDL_DisplayFormat(surface_background);
        SDL_FreeSurface(cleanup);
    }

    surface_title = IMG_Load(PKGDATADIR
#ifdef __GCW0__
        "/graphics/title_320x240.png");
#else
        "/graphics/title.png");
#endif
    if (!surface_title) return false;
    else {
        SDL_Surface *cleanup = surface_title;
        surface_title = SDL_DisplayFormatAlpha(surface_title);
        SDL_FreeSurface(cleanup);
    }

    surface_highscores = IMG_Load(PKGDATADIR "/graphics/highscores.png");
    if (!surface_highscores) return false;
    else {
        SDL_Surface *cleanup = surface_highscores;
        surface_highscores = SDL_DisplayFormatAlpha(surface_highscores);
        SDL_FreeSurface(cleanup);
    }

    // background music
    music = Mix_LoadMUS(PKGDATADIR "/sounds/music.ogg");
    if (!music) return false;

    // sound effects
    sound_menu = Mix_LoadWAV(PKGDATADIR "/sounds/menu.wav");
    if (!sound_menu) return false;

    sound_switch = Mix_LoadWAV(PKGDATADIR "/sounds/switch.wav");
    if (!sound_switch) return false;

    sound_match = Mix_LoadWAV(PKGDATADIR "/sounds/match.wav");
    if (!sound_match) return false;

    return true;
}

void sysCleanup() {
    sysConfigSave();
    if (config_folder) free(config_folder);

    Mix_HaltMusic();

    TTF_CloseFont(font);
    SDL_FreeSurface(surface_blocks);
    SDL_FreeSurface(surface_clear);
    SDL_FreeSurface(surface_cursor);
    SDL_FreeSurface(surface_bar);
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
            if(event.key.keysym.sym == KEY_SWITCH)
                action_switch = true;
            if(event.key.keysym.sym == KEY_BUMP)
                action_bump = true;
            if(event.key.keysym.sym == SDLK_ESCAPE)
                action_pause = true;
        }

        else if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_LEFT)
                action_left = false;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_right = false;
            if(event.key.keysym.sym == SDLK_UP)
                action_up = false;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_down = false;
            if(event.key.keysym.sym == KEY_SWITCH)
                action_switch = false;
            if(event.key.keysym.sym == KEY_BUMP)
                action_bump = false;
            if(event.key.keysym.sym == SDLK_ESCAPE)
                action_pause = false;
        }

        else if(event.type == SDL_JOYBUTTONDOWN) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_switch = true;
                if (event.jbutton.button == 1)
                    action_bump = true;
                if (event.jbutton.button == 9)
                    action_pause = true;
            }
        }

        else if(event.type == SDL_JOYBUTTONUP) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_switch = false;
                if (event.jbutton.button == 1)
                    action_bump = false;
                if (event.jbutton.button == 9)
                    action_pause = false;
            }
        }

        else if(event.type == SDL_QUIT) {
            quit = true;
        }
    }

    if (joy) {
        int joy_x = SDL_JoystickGetAxis(joy, 0);
        int joy_y = SDL_JoystickGetAxis(joy, 1);

        // x axis
        if (joy_x < -JOY_DEADZONE) {
            action_left = true;
            action_right = false;
        } else if (joy_x > JOY_DEADZONE) {
            action_left = false;
            action_right = true;
        } else {
            action_left = false;
            action_right = false;
        }

        // y axis
        if (joy_y < -JOY_DEADZONE) {
            action_up = true;
            action_down = false;
        } else if (joy_y > JOY_DEADZONE) {
            action_up = false;
            action_down = true;
        } else {
            action_up = false;
            action_down = false;
        }
    }
}

void sysConfigSetFolder() {
    char *home = getenv("HOME");

    config_folder = malloc(strlen(home)+strlen("/.freeblocks")+1);
    sprintf(config_folder,"%s/.freeblocks",home);
}

void sysConfigLoad() {
    if (!config_folder) return;

    FILE *config_file;
    char buffer[BUFSIZ];
    char *key;
    char *temp;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *config_path = malloc(strlen(config_folder)+strlen("/config")+1);

    if (config_path) {
        sprintf(config_path,"%s/config",config_folder);
        config_file = fopen(config_path,"r+");

        if (config_file) {
            while (fgets(buffer,BUFSIZ,config_file)) {
                temp = buffer;
                if (temp[0] == '#') continue;
                key = strtok(temp,"=");
                if (strcmp(key,"joystick") == 0) option_joystick = atoi(strtok(NULL,"\n"));
                if (strcmp(key,"sound") == 0) option_sound = atoi(strtok(NULL,"\n"));
                if (strcmp(key,"music") == 0) option_music = atoi(strtok(NULL,"\n"));
                if (strcmp(key,"fullscreen") == 0) option_fullscreen = atoi(strtok(NULL,"\n"));
            }
            fclose(config_file);
            sysConfigApply();
        } else {
            printf ("Error: Couldn't load config file. Creating new config...\n");
            sysConfigSave();
        }

        free(config_path);
    }
}

void sysConfigSave() {
    if (!config_folder) return;

    FILE *config_file;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *config_path = malloc(strlen(config_folder)+strlen("/config")+1);

    if (config_path) {
        sprintf(config_path,"%s/config",config_folder);
        config_file = fopen(config_path,"w+");

        if (config_file) {
            fprintf(config_file,"joystick=%d\n",option_joystick);
            fprintf(config_file,"sound=%d\n",option_sound);
            fprintf(config_file,"music=%d\n",option_music);
            fprintf(config_file,"fullscreen=%d\n",option_fullscreen);
            fclose(config_file);
        } else printf("Error: Couldn't write to config file.\n");

        free(config_path);
    }

    sysConfigApply();
}

void sysConfigApply() {
    if (joy) SDL_JoystickClose(joy);
    if (SDL_NumJoysticks() > 0 && option_joystick > -1) joy = SDL_JoystickOpen(option_joystick);
    else {
        option_joystick = -1;
        joy = NULL;
    }

    Mix_Volume(-1,option_sound*16);
    Mix_VolumeMusic(option_music*16);

    if (option_fullscreen == 1) {
        screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_FLAGS|SDL_FULLSCREEN);
    }
    if (!screen || option_fullscreen != 1) {
        screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_FLAGS);
    }
}

void sysHighScoresLoad() {
    if (!config_folder) return;

    FILE *file;
    char buffer[BUFSIZ];
    char *temp;
    int i = 0;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *path = malloc(strlen(config_folder)+strlen("/highscores")+1);

    if (path) {
        sprintf(path,"%s/highscores",config_folder);
        file = fopen(path,"r+");

        if (file) {
            while (fgets(buffer,BUFSIZ,file)) {
                temp = buffer;
                if (i < 10) high_scores[i] = atoi(strtok(temp,"\n"));
                else break;
                i++;
            }
            fclose(file);
        } else {
            printf ("Error: Couldn't load high scores.\n");
            sysHighScoresSave();
        }

        free(path);
    }
}

void sysHighScoresSave() {
    if (!config_folder) return;

    FILE *file;
    int i = 0;

    mkdir(config_folder, S_IRWXU | S_IRWXG | S_IRWXO);
    char *path = malloc(strlen(config_folder)+strlen("/highscores")+1);

    if (path) {
        sprintf(path,"%s/highscores",config_folder);
        file = fopen(path,"w+");

        if (file) {
            for (i=0;i<10;i++) {
                fprintf(file,"%d\n",high_scores[i]);
            }
            fclose(file);
        } else printf("Error: Couldn't save high scores.\n");

        free(path);
    }
}

void sysHighScoresClear() {
    for (int i=0; i<10; i++) {
        high_scores[i] = 0;
    }
}


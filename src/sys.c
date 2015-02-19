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
#include <sys/stat.h>
#include <sys/types.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "sys.h"

#ifdef __GCW0__
#define KEY_SWITCH SDLK_LCTRL
#define KEY_BUMP SDLK_LALT
#define KEY_PAUSE SDLK_RETURN
#define KEY_EXIT SDLK_ESCAPE
#define SDL_FLAGS (SDL_HWSURFACE|SDL_TRIPLEBUF)
#else
#define KEY_SWITCH 'z'
#define KEY_BUMP 'x'
#define KEY_PAUSE SDLK_ESCAPE
#define KEY_EXIT SDLK_ESCAPE
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
ActionMove action_move = ACTION_NONE;
ActionMove action_last_move = ACTION_NONE;
ActionMove action_switch = ACTION_NONE;
bool action_bump = false;
bool action_pause = false;
bool action_exit = false;

char* config_folder = NULL;
int option_joystick = -1;
int option_sound = 8;
int option_music = 8;

#ifdef __GCW0__
int option_fullscreen = 1;
#else
int option_fullscreen = 0;
#endif

bool sysInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
   
    screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_FLAGS);

    if(screen == NULL) return false;
    
    if(TTF_Init() == -1) return false;

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) return false;
    
    SDL_WM_SetCaption("FreeBlocks v0.4",NULL);

    sysConfigSetFolder();
    sysConfigLoad();

    sysHighScoresClear();
    sysHighScoresLoad();

    return true;
}

char* sysGetFilePath(Dork_String *dest, const char* path, bool is_gfx) {
    if (dest == NULL) return NULL;

    struct stat st;

    // try current directory
    Dork_StringClear(dest);
    Dork_StringAppend(dest, "./res");
    if (is_gfx) {
        Dork_StringAppend(dest, GFX_PREFIX);
    }
    Dork_StringAppend(dest, path);

    if (stat(Dork_StringGetData(dest), &st) == 0)
        return Dork_StringGetData(dest);

    // try install directory
    Dork_StringClear(dest);
    Dork_StringAppend(dest, PKGDATADIR);
    if (is_gfx) {
        Dork_StringAppend(dest, GFX_PREFIX);
    }
    Dork_StringAppend(dest, path);

    if (stat(Dork_StringGetData(dest), &st) == 0)
        return Dork_StringGetData(dest);

    // failure, just return NULL
    return NULL;
}

bool sysLoadImage(SDL_Surface** dest, const char* path) {
    Dork_String temp;
    Dork_StringInit(&temp);

    *dest = IMG_Load(sysGetFilePath(&temp, path, true));
    if (*dest == NULL) {
        Dork_StringClear(&temp);
        return false;
    }
    else {
        SDL_Surface *cleanup = *dest;
        *dest = SDL_DisplayFormatAlpha(cleanup);
        SDL_FreeSurface(cleanup);
    }

    Dork_StringClear(&temp);
    return true;
}

bool sysLoadFont(TTF_Font** dest, const char* path, int font_size) {
    Dork_String temp;
    Dork_StringInit(&temp);

    *dest = TTF_OpenFont(sysGetFilePath(&temp, path, false), font_size);
    if(*dest == NULL) {
        Dork_StringClear(&temp);
        return false;
    }
    else TTF_SetFontHinting(*dest, TTF_HINTING_LIGHT);

    Dork_StringClear(&temp);
    return true;
}

bool sysLoadMusic(Mix_Music** dest, const char* path) {
    Dork_String temp;
    Dork_StringInit(&temp);

    *dest = Mix_LoadMUS(sysGetFilePath(&temp, path, false));
    if (*dest == NULL) {
        Dork_StringClear(&temp);
        return false;
    }

    Dork_StringClear(&temp);
    return true;
}

bool sysLoadSound(Mix_Chunk** dest, const char* path) {
    Dork_String temp;
    Dork_StringInit(&temp);

    *dest = Mix_LoadWAV(sysGetFilePath(&temp, path, false));
    if (*dest == NULL) {
        Dork_StringClear(&temp);
        return false;
    }

    Dork_StringClear(&temp);
    return true;
}

bool sysLoadFiles() {
    // font
    if (!sysLoadFont(&font, "/fonts/Alegreya-Regular.ttf", FONT_SIZE)) return false;

    // graphics
    if (!sysLoadImage(&surface_blocks, "blocks.png")) return false;
    if (!sysLoadImage(&surface_clear, "clear.png")) return false;
#ifdef __JEWELS__
    if (!sysLoadImage(&surface_cursor, "cursor_single.png")) return false;
#else
    if (!sysLoadImage(&surface_cursor, "cursor.png")) return false;
#endif
    if (!sysLoadImage(&surface_bar, "bar.png")) return false;
    if (!sysLoadImage(&surface_bar_inactive, "bar_inactive.png")) return false;
    if (!sysLoadImage(&surface_background, "background.png")) return false;
    if (!sysLoadImage(&surface_title, "title.png")) return false;
    if (!sysLoadImage(&surface_highscores, "highscores.png")) return false;

    // background music
    if (!sysLoadMusic(&music, "/sounds/music.ogg")) return false;

    // sound effects
    if (!sysLoadSound(&sound_menu, "/sounds/menu.wav")) return false;
    if (!sysLoadSound(&sound_switch, "/sounds/switch.wav")) return false;
    if (!sysLoadSound(&sound_match, "/sounds/match.wav")) return false;

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
                action_move = ACTION_LEFT;
            if(event.key.keysym.sym == SDLK_RIGHT)
                action_move = ACTION_RIGHT;
            if(event.key.keysym.sym == SDLK_UP)
                action_move = ACTION_UP;
            if(event.key.keysym.sym == SDLK_DOWN)
                action_move = ACTION_DOWN;
#ifdef __JEWELS__
            if(event.key.keysym.sym == SDLK_a)
                action_switch = ACTION_LEFT;
            if(event.key.keysym.sym == SDLK_d)
                action_switch = ACTION_RIGHT;
            if(event.key.keysym.sym == SDLK_w)
                action_switch = ACTION_UP;
            if(event.key.keysym.sym == SDLK_s)
                action_switch = ACTION_DOWN;
#else
            if(event.key.keysym.sym == KEY_SWITCH)
                action_switch = ACTION_RIGHT;
#endif
            if(event.key.keysym.sym == KEY_BUMP)
                action_bump = true;
            if(event.key.keysym.sym == KEY_PAUSE)
                action_pause = true;
            if(event.key.keysym.sym == KEY_EXIT)
                action_exit = true;
        }

        else if(event.type == SDL_KEYUP) {
            if((event.key.keysym.sym == SDLK_LEFT && action_move == ACTION_LEFT) ||
               (event.key.keysym.sym == SDLK_RIGHT && action_move == ACTION_RIGHT) ||
               (event.key.keysym.sym == SDLK_UP && action_move == ACTION_UP) ||
               (event.key.keysym.sym == SDLK_DOWN && action_move == ACTION_DOWN)) {
                action_move = ACTION_NONE;
                action_last_move = ACTION_NONE;
            }
#ifdef __JEWELS__
            if(event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_s)
#else
			if(event.key.keysym.sym == KEY_SWITCH)
#endif
                action_switch = ACTION_NONE;
            if(event.key.keysym.sym == KEY_BUMP)
                action_bump = false;
            if(event.key.keysym.sym == KEY_PAUSE)
                action_pause = false;
            if(event.key.keysym.sym == KEY_EXIT)
                action_exit = false;
        }

        else if(event.type == SDL_JOYBUTTONDOWN) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_switch = ACTION_RIGHT;
                if (event.jbutton.button == 1)
                    action_bump = true;
                if (event.jbutton.button == 9)
                    action_pause = true;
            }
        }

        else if(event.type == SDL_JOYBUTTONUP) {
            if(event.jbutton.which == 0) {
                if (event.jbutton.button == 0)
                    action_switch = ACTION_NONE;
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

    if (joy && event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) {
        int joy_x = SDL_JoystickGetAxis(joy, 0);
        int joy_y = SDL_JoystickGetAxis(joy, 1);

        // x axis
        if (joy_x < -JOY_DEADZONE) {
            action_move = ACTION_LEFT;
        } else if (joy_x > JOY_DEADZONE) {
            action_move = ACTION_RIGHT;
        }

        // y axis
        if (joy_y < -JOY_DEADZONE) {
            action_move = ACTION_UP;
        } else if (joy_y > JOY_DEADZONE) {
            action_move = ACTION_DOWN;
        }

        if (joy_x >= -JOY_DEADZONE && joy_x <= JOY_DEADZONE && joy_y >= -JOY_DEADZONE && joy_y <= JOY_DEADZONE) {
            action_move = ACTION_NONE;
        }
    }
}

void sysConfigSetFolder() {
    char *home = getenv(HOME_DIR_ENV);

    config_folder = malloc(strlen(home)+strlen("/.freeblocks")+1);
    sprintf(config_folder,"%s/.freeblocks",home);
}

void sysConfigLoad() {
    if (!config_folder) return;

    FILE *config_file;
    char buffer[BUFSIZ];
    char *key;
    char *temp;

    mkdir(config_folder, MKDIR_MODE);
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

    mkdir(config_folder, MKDIR_MODE);
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

    mkdir(config_folder, MKDIR_MODE);
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

    mkdir(config_folder, MKDIR_MODE);
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


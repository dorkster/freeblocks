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

#ifndef SYS_H
#define SYS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "dork/dork_string.h"

typedef enum { false = 0, true = 1 } bool;

#define GAME_MODE_DEFAULT 0
#define GAME_MODE_JEWELS 1

#ifdef __GCW0__
#define HALF_GFX
#define SCREEN_BPP 16
#else
#define SCREEN_BPP 32
#endif

#ifdef HALF_GFX
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define GFX_PREFIX "/graphics/320x240/"
#define FONT_SIZE 12
#else
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GFX_PREFIX "/graphics/"
#define FONT_SIZE 24
#endif

#define FPS 30
#define JOY_DEADZONE 100
#define ACTION_COOLDOWN 10 / (60/FPS)

#define OPTIONS_MAIN 0
#define OPTIONS_JOYSTICK 1
#define OPTIONS_SOUND 2
#define OPTIONS_MUSIC 3

#ifndef PKGDATADIR
#define PKGDATADIR "./res"
#endif

#ifdef _MSC_VER
#define HOME_DIR_ENV "AppData"
#else
#define HOME_DIR_ENV "HOME"
#endif

#ifdef _MSC_VER
#define MKDIR_MODE 0
#define mkdir(p, a) _mkdir(p)
#else
#define MKDIR_MODE (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

#ifdef _MSC_VER
#include <direct.h>
#define mkdir(p, a) _mkdir(p)
#elif defined(_WIN32)
#define mkdir(p, a) mkdir(p)
#endif

#ifdef _MSC_VER
#define stat _stat
#endif

SDL_Surface* screen;
TTF_Font* font;

int score;
int high_scores[10];
bool title_screen;
bool high_scores_screen;
int options_screen;
bool game_over;
bool paused;
bool quit;
int game_mode;

struct Cursor {
    int x1;
    int y1;
    int x2;
    int y2;
}cursor;

int action_cooldown;
typedef enum {
    ACTION_NONE, ACTION_LEFT, ACTION_RIGHT, ACTION_UP, ACTION_DOWN
}ActionMove;
ActionMove action_move;
ActionMove action_last_move;
ActionMove action_switch;
bool action_bump;
bool action_pause;
bool action_exit;

Dork_String path_dir_config;
Dork_String path_file_config;
Dork_String path_file_highscores;
Dork_String path_file_highscores_jewels;

int option_joystick;
int option_sound;
int option_music;
int option_fullscreen;

SDL_Event event;

// Timers
unsigned int startTimer;
unsigned int endTimer;
unsigned int deltaTimer;

// Functions
bool sysInit();
char* sysGetFilePath(Dork_String *dest, const char* path, bool is_gfx);
bool sysLoadImage(SDL_Surface** dest, const char* path);
bool sysLoadFont(TTF_Font** dest, const char* path, int font_size);
bool sysLoadMusic(Mix_Music** dest, const char* path);
bool sysLoadSound(Mix_Chunk** dest, const char* path);
bool sysLoadFiles();
void sysCleanup();
void sysInput();
void sysConfigSetPaths();
void sysConfigLoad();
void sysConfigSave();
void sysConfigApply();
void sysHighScoresLoad();
void sysHighScoresSave();
void sysHighScoresClear();

// Images
SDL_Surface* surface_blocks;
SDL_Surface* surface_clear;
SDL_Surface* surface_cursor;
SDL_Surface* surface_cursor_highlight;
SDL_Surface* surface_bar;
SDL_Surface* surface_bar_inactive;
SDL_Surface* surface_background;
SDL_Surface* surface_background_jewels;
SDL_Surface* surface_title;
SDL_Surface* surface_highscores;

// Music and Sounds
Mix_Music* music;
Mix_Music* music_jewels;
Mix_Chunk* sound_menu;
Mix_Chunk* sound_switch;
Mix_Chunk* sound_match;

// Joystick
SDL_Joystick* joy;

#endif

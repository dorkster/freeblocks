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

#define FPS 60
#define JOY_DEADZONE 100
#define ACTION_COOLDOWN 10 / (60/FPS)

#define OPTIONS_MAIN 0
#define OPTIONS_CONTROLS 1
#define OPTIONS_REBIND 2

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

enum KEYBINDS {
    KEY_SWITCH = 0,
    KEY_BUMP = 1,
    KEY_ACCEPT = 2,
    KEY_PAUSE = 3,
    KEY_EXIT = 4,
    KEY_LEFT = 5,
    KEY_RIGHT = 6,
    KEY_UP = 7,
    KEY_DOWN = 8
};

extern const char* const key_desc[];

typedef struct Image {
    SDL_Texture* texture;
    int w;
    int h;
}Image;

SDL_Window* window;
SDL_Renderer* renderer;
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
bool action_accept;
bool action_pause;
bool action_exit;
bool action_click;

Dork_String path_dir_config;
Dork_String path_file_config;
Dork_String path_file_highscores;
Dork_String path_file_highscores_jewels;

int option_joystick;
int option_sound;
int option_music;
int option_fullscreen;

SDL_Keycode option_key[9];
int option_joy_button[5];
int option_joy_axis_x;
int option_joy_axis_y;

SDL_Keycode last_key;
int last_joy_button;

SDL_Event event;
int mouse_x;
int mouse_y;

// Timers
unsigned int startTimer;
unsigned int endTimer;
unsigned int deltaTimer;

// Functions
void sysInitVars();
bool sysInit();
char* sysGetFilePath(Dork_String *dest, const char* path, bool is_gfx);
bool sysLoadImage(Image** dest, const char* path);
void sysDestroyImage(Image** dest);
void sysRenderImage(Image* img, SDL_Rect* src, SDL_Rect* dest);
bool sysLoadFont(TTF_Font** dest, const char* path, int font_size);
bool sysLoadMusic(Mix_Music** dest, const char* path);
bool sysLoadSound(Mix_Chunk** dest, const char* path);
bool sysLoadFiles();
void sysCleanup();
void sysInput();
void sysInputReset();
void sysConfigSetPaths();
void sysConfigLoad();
void sysConfigSave();
void sysConfigApply();
void sysHighScoresLoad();
void sysHighScoresSave();
void sysHighScoresClear();
void logInfo(const char* format, ...);
void logError(const char* format, ...);

// Images
Image* surface_blocks;
Image* surface_clear;
Image* surface_cursor;
Image* surface_cursor_highlight;
Image* surface_bar;
Image* surface_bar_inactive;
Image* surface_background;
Image* surface_background_jewels;
Image* surface_title;
Image* surface_highscores;

// Music and Sounds
Mix_Music* music;
Mix_Music* music_jewels;
Mix_Chunk* sound_menu;
Mix_Chunk* sound_switch;
Mix_Chunk* sound_match;
Mix_Chunk* sound_drop;

// Joystick
SDL_Joystick* joy;

#endif

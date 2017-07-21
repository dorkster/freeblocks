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

#ifndef SYS_H
#define SYS_H

#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef __EMSCRIPTEN__
#include <SDL/SDL_mixer.h>
bool emscripten_can_write;
bool emscriptenPersistData();
#else
#include <SDL_mixer.h>
#endif

#include "string.h"

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
#define JOY_DEADZONE 8192
#define ACTION_COOLDOWN 10 / (60/FPS)

#define OPTIONS_MAIN 0
#define OPTIONS_CONTROLS 1
#define OPTIONS_REBIND 2

#ifndef PKGDATADIR
#define PKGDATADIR "./res"
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

#ifndef min
#define min(a, b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#define KEY_COUNT 10

enum KEYBINDS {
    KEY_SWITCH = 0,
    KEY_BUMP = 1,
    KEY_PICKUP = 2,
    KEY_ACCEPT = 3,
    KEY_PAUSE = 4,
    KEY_EXIT = 5,
    KEY_LEFT = 6,
    KEY_RIGHT = 7,
    KEY_UP = 8,
    KEY_DOWN = 9
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
bool force_pause;
bool quit;

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
bool action_switch;
bool action_bump;
bool action_pickup;
bool action_accept;
bool action_pause;
bool action_exit;
bool action_click;
bool action_right_click;

String path_dir_config;
String path_file_config;
String path_file_highscores;
String path_file_highscores_jewels;
String path_file_highscores_drop;

int option_joystick;
int option_sound;
int option_music;
int option_fullscreen;

SDL_Keycode option_key[KEY_COUNT];
int option_joy_button[KEY_COUNT-4]; // joysticks can't remap directions
int option_joy_axis_x;
int option_joy_axis_y;

SDL_Keycode last_key;
int last_joy_button;

SDL_Event event;
int mouse_x;
int mouse_y;
bool mouse_moving;

// Timers
unsigned int startTimer;
unsigned int endTimer;
unsigned int deltaTimer;

// Functions
void sysInitVars();
bool sysInit();
char* sysGetFilePath(String *dest, const char* path, bool is_gfx);
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
Image* img_blocks;
Image* img_clear;
Image* img_cursor;
Image* img_cursor_highlight;
Image* img_bar;
Image* img_bar_inactive;
Image* img_bar_left;
Image* img_bar_right;
Image* img_background;
Image* img_background_jewels;
Image* img_background_drop;
Image* img_title;
Image* img_highscores;

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

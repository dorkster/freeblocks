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
#include <stdarg.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "sys.h"

#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC)

#define VERSION "0.5"
#define WINDOW_TITLE "FreeBlocks v" VERSION

const char* const key_desc[] = {
    "Switch blocks",
    "Raise blocks",
    "Accept",
    "Pause",
    "Exit",
    "Left",
    "Right",
    "Up",
    "Down"
};

void sysInitVars() {
    window = NULL;
    renderer = NULL;
    font = NULL;
    surface_blocks = NULL;
    surface_clear = NULL;
    surface_cursor = NULL;
    surface_cursor_highlight = NULL;
    surface_bar = NULL;
    surface_bar_inactive = NULL;
    surface_bar_left = NULL;
    surface_bar_right = NULL;
    surface_background = NULL;
    surface_background_jewels = NULL;
    surface_title = NULL;
    surface_highscores = NULL;
    music = NULL;
    music_jewels = NULL;
    sound_menu = NULL;
    sound_switch = NULL;
    sound_match = NULL;
    sound_drop = NULL;
    joy = NULL;

    score = 0;
    title_screen = true;
    high_scores_screen = false;
    options_screen = -1;
    game_over = false;
    paused = false;
    force_pause = false;
    quit = false;

    game_mode = GAME_MODE_DEFAULT;

    action_cooldown = 0;
    action_move = ACTION_NONE;
    action_last_move = ACTION_NONE;
    action_switch = ACTION_NONE;
    action_bump = false;
    action_accept = false;
    action_pause = false;
    action_exit = false;
    action_click = false;

    option_joystick = -1;
    option_sound = 8;
    option_music = 8;

#ifdef __GCW0__
    option_fullscreen = 1;
#else
    option_fullscreen = 0;
#endif

    last_key = SDLK_UNKNOWN;
    last_joy_button = -1;

    mouse_x = 0;
    mouse_y = 0;
}

bool sysInit() {
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return false;
    if(TTF_Init() == -1) return false;
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) return false;

    sysInitVars();

    // set up the default controls
    option_key[KEY_SWITCH] = SDLK_LCTRL;
    option_key[KEY_BUMP] = SDLK_LALT;
    option_key[KEY_ACCEPT] = SDLK_RETURN;
#ifdef __ANDROID__
    option_key[KEY_EXIT] = SDLK_AC_BACK;
#else
    option_key[KEY_EXIT] = SDLK_ESCAPE;
#endif

    option_key[KEY_LEFT] = SDLK_LEFT;
    option_key[KEY_RIGHT] = SDLK_RIGHT;
    option_key[KEY_UP] = SDLK_UP;
    option_key[KEY_DOWN] = SDLK_DOWN;

    option_key[KEY_PAUSE] = SDLK_ESCAPE;
#ifdef __GCW0__
    option_key[KEY_PAUSE] = SDLK_RETURN;
#endif
#ifdef __ANDROID__
    option_key[KEY_PAUSE] = SDLK_AC_BACK;
#endif

    option_joy_button[KEY_SWITCH] = 0;
    option_joy_button[KEY_BUMP] = 1;
    option_joy_button[KEY_ACCEPT] = 9;
    option_joy_button[KEY_PAUSE] = 9;
    option_joy_button[KEY_EXIT] = 8;

    option_joy_axis_x = 0;
    option_joy_axis_y = 1;

    // load config
    sysConfigSetPaths();
    sysConfigLoad();
    sysConfigApply();

    //load high scores
    sysHighScoresClear();
    sysHighScoresLoad();

    return true;
}

char* sysGetFilePath(Dork_String *dest, const char* path, bool is_gfx) {
    if (dest == NULL) return NULL;

    struct stat st;

    Dork_StringClear(dest);

#ifdef __ANDROID__
    Dork_StringAppend(dest, "res");
#else
    Dork_StringAppend(dest, "./res");
#endif

    // try current directory
    if (is_gfx) {
        Dork_StringAppend(dest, GFX_PREFIX);
    }
    Dork_StringAppend(dest, path);

#ifdef __ANDROID__
    // can't stat internal Android storage
    return Dork_StringGetData(dest);
#else
    if (stat(Dork_StringGetData(dest), &st) == 0)
        return Dork_StringGetData(dest);
#endif

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

bool sysLoadImage(Image** dest, const char* path) {
    Dork_String temp;
    Dork_StringInit(&temp);

    SDL_Surface* surface = IMG_Load(sysGetFilePath(&temp, path, true));
    Dork_StringClear(&temp);

    if (surface == NULL) {
        return false;
    }
    else {
        *dest = malloc(sizeof(Image));
        if (*dest == NULL)
            return false;

        (*dest)->w = 0;
        (*dest)->h = 0;
        (*dest)->texture = NULL;

        (*dest)->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        SDL_QueryTexture((*dest)->texture, NULL, NULL, &((*dest)->w), &((*dest)->h));
    }

    return true;
}

void sysDestroyImage(Image** dest) {
    if (*dest != NULL) {
        SDL_DestroyTexture((*dest)->texture);
        free(*dest);
        *dest = NULL;
    }
}

void sysRenderImage(Image* img, SDL_Rect* src, SDL_Rect* dest) {
    if (!img) return;

    if (dest) {
        if (src) {
            dest->w = src->w;
            dest->h = src->h;
        }
        else {
            dest->w = img->w;
            dest->h = img->h;
        }
    }

    SDL_RenderCopy(renderer, img->texture, src, dest);
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
    if (!sysLoadImage(&surface_cursor, "cursor.png")) return false;
    if (!sysLoadImage(&surface_cursor_highlight, "cursor_highlight.png")) return false;
    if (!sysLoadImage(&surface_bar, "bar.png")) return false;
    if (!sysLoadImage(&surface_bar_inactive, "bar_inactive.png")) return false;
    if (!sysLoadImage(&surface_bar_left, "bar_left.png")) return false;
    if (!sysLoadImage(&surface_bar_right, "bar_right.png")) return false;
    if (!sysLoadImage(&surface_background, "background.png")) return false;
    if (!sysLoadImage(&surface_background_jewels, "background_jewels.png")) return false;
    if (!sysLoadImage(&surface_title, "title.png")) return false;
    if (!sysLoadImage(&surface_highscores, "highscores.png")) return false;

    // background music
    if (!sysLoadMusic(&music, "/sounds/music.ogg")) return false;
    if (!sysLoadMusic(&music_jewels, "/sounds/music_jewels.ogg")) return false;

    // sound effects
    if (!sysLoadSound(&sound_menu, "/sounds/menu.wav")) return false;
    if (!sysLoadSound(&sound_switch, "/sounds/switch.wav")) return false;
    if (!sysLoadSound(&sound_match, "/sounds/match.wav")) return false;
    if (!sysLoadSound(&sound_drop, "/sounds/drop.wav")) return false;

    return true;
}

void sysCleanup() {
    sysConfigSave();

    Dork_StringClear(&path_dir_config);
    Dork_StringClear(&path_file_config);
    Dork_StringClear(&path_file_highscores);
    Dork_StringClear(&path_file_highscores_jewels);

    Mix_HaltMusic();

    TTF_CloseFont(font);

    sysDestroyImage(&surface_blocks);
    sysDestroyImage(&surface_clear);
    sysDestroyImage(&surface_cursor);
    sysDestroyImage(&surface_cursor_highlight);
    sysDestroyImage(&surface_bar);
    sysDestroyImage(&surface_bar_inactive);
    sysDestroyImage(&surface_bar_left);
    sysDestroyImage(&surface_bar_right);
    sysDestroyImage(&surface_background);
    sysDestroyImage(&surface_background_jewels);
    sysDestroyImage(&surface_title);
    sysDestroyImage(&surface_highscores);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_FreeMusic(music);
    Mix_FreeMusic(music_jewels);

    Mix_FreeChunk(sound_menu);
    Mix_FreeChunk(sound_switch);
    Mix_FreeChunk(sound_match);
    Mix_FreeChunk(sound_drop);

    Mix_CloseAudio();

    SDL_Quit();
}

void sysInput() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEMOTION) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            if (event.button.button == SDL_BUTTON_LEFT)
                action_click = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            if (event.button.button == SDL_BUTTON_LEFT)
                action_click = false;
        }
        else if (event.type == SDL_FINGERMOTION) {
            mouse_x = (int)((event.tfinger.x + event.tfinger.dx) * SCREEN_WIDTH);
            mouse_y = (int)((event.tfinger.y + event.tfinger.dy) * SCREEN_HEIGHT);
        }
        else if (event.type == SDL_KEYDOWN) {
            last_key = event.key.keysym.sym;

            if (event.key.keysym.sym == option_key[KEY_LEFT])
                action_move = ACTION_LEFT;
            if (event.key.keysym.sym == option_key[KEY_RIGHT])
                action_move = ACTION_RIGHT;
            if (event.key.keysym.sym == option_key[KEY_UP])
                action_move = ACTION_UP;
            if (event.key.keysym.sym == option_key[KEY_DOWN])
                action_move = ACTION_DOWN;
            if (event.key.keysym.sym == option_key[KEY_SWITCH])
                action_switch = true;
            if (event.key.keysym.sym == option_key[KEY_BUMP])
                action_bump = true;
            if (event.key.keysym.sym == option_key[KEY_ACCEPT])
                action_accept = true;
            if (event.key.keysym.sym == option_key[KEY_PAUSE])
                action_pause = true;
            if (event.key.keysym.sym == option_key[KEY_EXIT])
                action_exit = true;
        }
        else if (event.type == SDL_KEYUP) {
            if ((event.key.keysym.sym == option_key[KEY_LEFT] && action_move == ACTION_LEFT) ||
               (event.key.keysym.sym == option_key[KEY_RIGHT] && action_move == ACTION_RIGHT) ||
               (event.key.keysym.sym == option_key[KEY_UP] && action_move == ACTION_UP) ||
               (event.key.keysym.sym == option_key[KEY_DOWN] && action_move == ACTION_DOWN)) {
                action_move = ACTION_NONE;
                action_last_move = ACTION_NONE;
            }
            if (event.key.keysym.sym == option_key[KEY_SWITCH])
                action_switch = false;
            if (event.key.keysym.sym == option_key[KEY_BUMP])
                action_bump = false;
            if (event.key.keysym.sym == option_key[KEY_ACCEPT])
                action_accept = false;
            if (event.key.keysym.sym == option_key[KEY_PAUSE])
                action_pause = false;
            if (event.key.keysym.sym == option_key[KEY_EXIT])
                action_exit = false;
        }
        else if (option_joystick > -1 && event.type == SDL_JOYBUTTONDOWN) {
            if (event.jbutton.which == option_joystick) {
                last_joy_button = event.jbutton.button;

                if (event.jbutton.button == option_joy_button[KEY_SWITCH])
                    action_switch = true;
                if (event.jbutton.button == option_joy_button[KEY_BUMP])
                    action_bump = true;
                if (event.jbutton.button == option_joy_button[KEY_ACCEPT])
                    action_accept = true;
                if (event.jbutton.button == option_joy_button[KEY_PAUSE])
                    action_pause = true;
                if (event.jbutton.button == option_joy_button[KEY_EXIT])
                    action_exit = true;
            }
        }
        else if (option_joystick > -1 && event.type == SDL_JOYBUTTONUP) {
            if (event.jbutton.which == option_joystick) {
                if (event.jbutton.button == option_joy_button[KEY_SWITCH])
                    action_switch = false;
                if (event.jbutton.button == option_joy_button[KEY_BUMP])
                    action_bump = false;
                if (event.jbutton.button == option_joy_button[KEY_ACCEPT])
                    action_accept = false;
                if (event.jbutton.button == option_joy_button[KEY_PAUSE])
                    action_pause = false;
                if (event.jbutton.button == option_joy_button[KEY_EXIT])
                    action_exit = false;
            }
        }
        else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                Mix_Pause(-1);
                Mix_PauseMusic();
                force_pause = true;
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESTORED) {
                Mix_Resume(-1);
                Mix_ResumeMusic();
            }
        }
        else if (event.type == SDL_QUIT) {
            quit = true;
        }
    }

    if (joy && event.type == SDL_JOYAXISMOTION) {
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

void sysInputReset() {
    action_move = ACTION_NONE;
    action_switch = false;
    action_bump = false;
    action_accept = false;
    action_pause = false;
    action_exit = false;
}

void sysConfigSetPaths() {
    Dork_StringInit(&path_dir_config);
#ifdef __ANDROID__
    Dork_StringAppend(&path_dir_config, SDL_AndroidGetInternalStoragePath());
#else
    Dork_StringAppend(&path_dir_config, getenv(HOME_DIR_ENV));
#endif
    Dork_StringAppend(&path_dir_config, "/.freeblocks");

    Dork_StringInit(&path_file_config);
    Dork_StringAppend(&path_file_config, Dork_StringGetData(&path_dir_config));
    Dork_StringAppend(&path_file_config, "/config");

    Dork_StringInit(&path_file_highscores);
    Dork_StringAppend(&path_file_highscores, Dork_StringGetData(&path_dir_config));
    Dork_StringAppend(&path_file_highscores, "/highscores");

    Dork_StringInit(&path_file_highscores_jewels);
    Dork_StringAppend(&path_file_highscores_jewels, Dork_StringGetData(&path_dir_config));
    Dork_StringAppend(&path_file_highscores_jewels, "/highscores_jewels");
}

void sysConfigLoad() {
    char buffer[BUFSIZ];
    char *key = NULL;
    char *temp = NULL;
    bool version_match = false;

    mkdir(Dork_StringGetData(&path_dir_config), MKDIR_MODE);

    FILE *config_file = fopen(Dork_StringGetData(&path_file_config),"r+");
    if (config_file) {
        while (fgets(buffer,BUFSIZ,config_file)) {
            temp = buffer;
            if (temp[0] == '#') continue;
            key = strtok(temp,"=");

            // check to see if this config matches our program version
            if (strcmp(key,"version") == 0) {
                if (strcmp(strtok(NULL, "\n"), VERSION) == 0)
                    version_match = true;
            }

            if (!version_match)
                continue;

            if (strcmp(key,"joystick") == 0) option_joystick = atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"sound") == 0) option_sound = atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"music") == 0) option_music = atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"fullscreen") == 0) option_fullscreen = atoi(strtok(NULL,"\n"));

#ifndef __ANDROID__
            else if (strcmp(key,"key_switch") == 0) option_key[KEY_SWITCH] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_bump") == 0) option_key[KEY_BUMP] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_accept") == 0) option_key[KEY_ACCEPT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_pause") == 0) option_key[KEY_PAUSE] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_exit") == 0) option_key[KEY_EXIT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_left") == 0) option_key[KEY_LEFT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_right") == 0) option_key[KEY_RIGHT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_up") == 0) option_key[KEY_UP] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"key_down") == 0) option_key[KEY_DOWN] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
#endif

            else if (strcmp(key,"joy_switch") == 0) option_joy_button[KEY_SWITCH] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"joy_bump") == 0) option_joy_button[KEY_BUMP] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"joy_accept") == 0) option_joy_button[KEY_ACCEPT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"joy_pause") == 0) option_joy_button[KEY_PAUSE] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"joy_exit") == 0) option_joy_button[KEY_EXIT] = (SDL_Keycode)atoi(strtok(NULL,"\n"));

            else if (strcmp(key,"joy_axis_x") == 0) option_joy_axis_x = (SDL_Keycode)atoi(strtok(NULL,"\n"));
            else if (strcmp(key,"joy_axis_y") == 0) option_joy_axis_y = (SDL_Keycode)atoi(strtok(NULL,"\n"));
        }
        fclose(config_file);
        sysConfigApply();
    } else {
        printf("Error: Couldn't load config file. Creating new config...\n");
        sysConfigSave();
        return;
    }

    if (!version_match) {
        printf("Error: Config file did not match current version. Creating new config...\n");
        sysConfigSave();
    }
}

void sysConfigSave() {
    mkdir(Dork_StringGetData(&path_dir_config), MKDIR_MODE);

    FILE *config_file = fopen(Dork_StringGetData(&path_file_config),"w+");
    if (config_file) {
        fprintf(config_file,"version=%s\n", VERSION);

        fprintf(config_file,"joystick=%d\n",option_joystick);
        fprintf(config_file,"sound=%d\n",option_sound);
        fprintf(config_file,"music=%d\n",option_music);
        fprintf(config_file,"fullscreen=%d\n",option_fullscreen);

        fprintf(config_file,"\n# keyboard/GCW-Zero bindings\n");
        fprintf(config_file,"key_switch=%d\n",(int)option_key[KEY_SWITCH]);
        fprintf(config_file,"key_bump=%d\n",(int)option_key[KEY_BUMP]);
        fprintf(config_file,"key_accept=%d\n",(int)option_key[KEY_ACCEPT]);
        fprintf(config_file,"key_pause=%d\n",(int)option_key[KEY_PAUSE]);
        fprintf(config_file,"key_exit=%d\n",(int)option_key[KEY_EXIT]);
        fprintf(config_file,"key_left=%d\n",(int)option_key[KEY_LEFT]);
        fprintf(config_file,"key_right=%d\n",(int)option_key[KEY_RIGHT]);
        fprintf(config_file,"key_up=%d\n",(int)option_key[KEY_UP]);
        fprintf(config_file,"key_down=%d\n",(int)option_key[KEY_DOWN]);

        fprintf(config_file,"\n# joystick bindings\n");
        fprintf(config_file,"joy_switch=%d\n",(int)option_joy_button[KEY_SWITCH]);
        fprintf(config_file,"joy_bump=%d\n",(int)option_joy_button[KEY_BUMP]);
        fprintf(config_file,"joy_accept=%d\n",(int)option_joy_button[KEY_ACCEPT]);
        fprintf(config_file,"joy_pause=%d\n",(int)option_joy_button[KEY_PAUSE]);
        fprintf(config_file,"joy_exit=%d\n",(int)option_joy_button[KEY_EXIT]);
        fprintf(config_file,"joy_axis_x=%d\n",option_joy_axis_x);
        fprintf(config_file,"joy_axis_y=%d\n",option_joy_axis_y);

        fclose(config_file);
    } else printf("Error: Couldn't write to config file.\n");

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

#ifdef __ANDROID__
    option_fullscreen = 1;
#endif

    if (option_fullscreen == 1) {
        if (!window)
            window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
        else
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else {
        if (!window) {
            window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        }
        else {
            SDL_SetWindowFullscreen(window, 0);
            SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    if (window && !renderer) {
        renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
    }
    if (renderer) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    if (!window || !renderer) {
        sysCleanup();
        exit(1);
    }
}

void sysHighScoresLoad() {
    FILE *file = NULL;
    char buffer[BUFSIZ];
    char *temp;
    int i = 0;

    mkdir(Dork_StringGetData(&path_dir_config), MKDIR_MODE);

    if (game_mode == GAME_MODE_JEWELS)
        file = fopen(Dork_StringGetData(&path_file_highscores_jewels),"r+");
    else
        file = fopen(Dork_StringGetData(&path_file_highscores),"r+");

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
        for (int j=0; j<10; j++) {
            high_scores[j] = 0;
        }
        sysHighScoresSave();
    }
}

void sysHighScoresSave() {
    FILE *file = NULL;
    int i = 0;

    mkdir(Dork_StringGetData(&path_dir_config), MKDIR_MODE);

    if (game_mode == GAME_MODE_JEWELS)
        file = fopen(Dork_StringGetData(&path_file_highscores_jewels),"w+");
    else
        file = fopen(Dork_StringGetData(&path_file_highscores),"w+");

    if (file) {
        for (i=0;i<10;i++) {
            fprintf(file,"%d\n",high_scores[i]);
        }
        fclose(file);
    } else printf("Error: Couldn't save high scores.\n");
}

void sysHighScoresClear() {
    for (int i=0; i<10; i++) {
        high_scores[i] = 0;
    }
}

void logInfo(const char* format, ...) {
	va_list args;

	va_start(args, format);

	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, args);

	va_end(args);
}

void logError(const char* format, ...) {
	va_list args;

	va_start(args, format);

	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, format, args);

	va_end(args);
}


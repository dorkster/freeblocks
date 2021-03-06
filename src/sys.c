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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "sys.h"
#include "game_mode.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
bool emscripten_fs_ready = false;

void emscriptenFSInit() {
    EM_ASM(
        FS.mkdir('/persistent_data');
        FS.mount(IDBFS,{},'/persistent_data');

        Module.print("start file sync..");
        Module.syncdone = 0;

        FS.syncfs(true, function(err) {
                       assert(!err);
                       Module.print("end file sync..");
                       Module.syncdone = 1;
        });
    );
}

void emscriptenLoadConfigs() {
    sysConfigLoad();

    GameMode *game_mode_current = game_mode;

    game_mode = &game_mode_default;
    sysHighScoresLoad();
    game_mode = &game_mode_jewels;
    sysHighScoresLoad();
    game_mode = &game_mode_drop;
    sysHighScoresLoad();

    game_mode = game_mode_current;
}

bool emscriptenPersistData() {
    if (emscripten_fs_ready)
        return true;

    if(emscripten_run_script_int("Module.syncdone") == 1) {
        FILE *config_file = fopen(path_file_config.buf,"r+");
        if (config_file == NULL) {
            //persist Emscripten current data to Indexed Db
            EM_ASM(
                Module.print("Start File sync..");
                Module.syncdone = 0;
                FS.syncfs(false, function(err) {
                    assert(!err);
                    Module.print("End File sync..");
                    Module.syncdone = 1;
                });
            );
            emscriptenLoadConfigs();
            return false;
        }
        else {
            fclose(config_file);
            emscripten_fs_ready = true;
            emscriptenLoadConfigs();
            return true;
        }
    }
    return false;
}

void emscriptenWriteFile(char* filename) {
    printf("Wrote to file: %s\n", filename);

    EM_ASM(
        //persist changes
        FS.syncfs(false,function (err) {
                          assert(!err);
        });
    );
}
#endif

#define RENDERER_FLAGS (SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC)

#define VERSION "0.5"
#define WINDOW_TITLE "FreeBlocks v" VERSION

const char* const key_desc[] = {
    "Switch blocks",
    "Raise blocks",
    "Pick up blocks",
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
    img_blocks = NULL;
    img_clear = NULL;
    img_cursor = NULL;
    img_cursor_highlight = NULL;
    img_bar = NULL;
    img_bar_inactive = NULL;
    img_bar_left = NULL;
    img_bar_right = NULL;
    img_background = NULL;
    img_background_jewels = NULL;
    img_background_drop = NULL;
    img_title = NULL;
    img_highscores = NULL;
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

    action_cooldown = 0;
    action_move = ACTION_NONE;
    action_last_move = ACTION_NONE;
    action_switch = false;
    action_bump = false;
    action_pickup = false;
    action_accept = false;
    action_pause = false;
    action_exit = false;
    action_click = false;
    action_right_click = false;

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
    mouse_moving = false;
}

bool sysInit() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) == -1) {
        logError("SDL_Init failied");
        return false;
    }
    if(TTF_Init() == -1) {
        logError("TTF_Init failed");
        return false;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1 ) {
        logError("Mix_OpenAudio failied");
        return false;
    }

    sysInitVars();

    // set up the default controls
    option_key[KEY_SWITCH] = SDLK_LCTRL;
    option_key[KEY_BUMP] = SDLK_LALT;
    option_key[KEY_PICKUP] = SDLK_LSHIFT;
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
    option_joy_button[KEY_PICKUP] = 2;
    option_joy_button[KEY_ACCEPT] = 9;
    option_joy_button[KEY_PAUSE] = 9;
    option_joy_button[KEY_EXIT] = 8;

    option_joy_axis_x = 0;
    option_joy_axis_y = 1;

    // load config
    sysConfigSetPaths();

#ifdef __EMSCRIPTEN__
    // set up persistent storage for Emscripten
    // Emscripten can't load until the FS is synced
    emscriptenFSInit();
#else 
    sysConfigLoad();
#endif

    sysConfigApply();

    return true;
}

char* sysGetFilePath(String *full_path, const char* path, bool is_gfx) {
    if (full_path == NULL) return NULL;

    struct stat st;

#ifdef __ANDROID__
    String_Init(full_path, "res", 0);
#else
    String_Init(full_path, "./res", 0);
#endif

    // try current directory
    if (is_gfx) {
        String_Append(full_path, GFX_PREFIX, 0);
    }
    String_Append(full_path, path, 0);

#ifdef __ANDROID__
    // can't stat internal Android storage
    return full_path->buf;
#else
    if (stat(full_path->buf, &st) == 0)
        return full_path->buf;
#endif

    // try install directory
    String_Clear(full_path);
    String_Init(full_path, PKGDATADIR, 0);
    if (is_gfx) {
        String_Append(full_path, GFX_PREFIX, 0);
    }
    String_Append(full_path, path, 0);

    if (stat(full_path->buf, &st) == 0)
        return full_path->buf;

    // failure, just return NULL
    return NULL;
}

bool sysLoadImage(Image** dest, const char* path) {
    String temp;
    SDL_Surface* surface = IMG_Load(sysGetFilePath(&temp, path, true));
    String_Clear(&temp);

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
    String temp;
    *dest = TTF_OpenFont(sysGetFilePath(&temp, path, false), font_size);
    String_Clear(&temp);

    if(*dest == NULL)
        return false;
    else
        TTF_SetFontHinting(*dest, TTF_HINTING_LIGHT);

    return true;
}

bool sysLoadMusic(Mix_Music** dest, const char* path) {
    String temp;
    *dest = Mix_LoadMUS(sysGetFilePath(&temp, path, false));
    String_Clear(&temp);

    if (*dest == NULL) {
        return false;
    }

    return true;
}

bool sysLoadSound(Mix_Chunk** dest, const char* path) {
    String temp;
    *dest = Mix_LoadWAV(sysGetFilePath(&temp, path, false));
    String_Clear(&temp);

    if (*dest == NULL) {
        return false;
    }

    return true;
}

bool sysLoadFiles() {
    // font
    if (!sysLoadFont(&font, "/fonts/Alegreya-Regular.ttf", FONT_SIZE)) return false;

    // graphics
    if (!sysLoadImage(&img_blocks, "blocks.png")) return false;
    if (!sysLoadImage(&img_clear, "clear.png")) return false;
    if (!sysLoadImage(&img_cursor, "cursor.png")) return false;
    if (!sysLoadImage(&img_cursor_highlight, "cursor_highlight.png")) return false;
    if (!sysLoadImage(&img_bar, "bar.png")) return false;
    if (!sysLoadImage(&img_bar_inactive, "bar_inactive.png")) return false;
    if (!sysLoadImage(&img_bar_left, "bar_left.png")) return false;
    if (!sysLoadImage(&img_bar_right, "bar_right.png")) return false;
    if (!sysLoadImage(&img_background, "background.png")) return false;
    if (!sysLoadImage(&img_background_jewels, "background_jewels.png")) return false;
    if (!sysLoadImage(&img_background_drop, "background_drop.png")) return false;
    if (!sysLoadImage(&img_title, "title.png")) return false;
    if (!sysLoadImage(&img_highscores, "highscores.png")) return false;

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

    String_Clear(&path_dir_config);
    String_Clear(&path_file_config);
    String_Clear(&path_file_highscores);
    String_Clear(&path_file_highscores_jewels);

    Mix_HaltMusic();

    TTF_CloseFont(font);

    sysDestroyImage(&img_blocks);
    sysDestroyImage(&img_clear);
    sysDestroyImage(&img_cursor);
    sysDestroyImage(&img_cursor_highlight);
    sysDestroyImage(&img_bar);
    sysDestroyImage(&img_bar_inactive);
    sysDestroyImage(&img_bar_left);
    sysDestroyImage(&img_bar_right);
    sysDestroyImage(&img_background);
    sysDestroyImage(&img_background_jewels);
    sysDestroyImage(&img_background_drop);
    sysDestroyImage(&img_title);
    sysDestroyImage(&img_highscores);

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
    mouse_moving = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEMOTION) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            mouse_moving = true;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            if (event.button.button == SDL_BUTTON_LEFT)
                action_click = true;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                action_right_click = true;
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            if (event.button.button == SDL_BUTTON_LEFT)
                action_click = false;
            else if (event.button.button == SDL_BUTTON_RIGHT)
                action_right_click = false;
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
            if (event.key.keysym.sym == option_key[KEY_PICKUP])
                action_pickup = true;
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
            if (event.key.keysym.sym == option_key[KEY_PICKUP])
                action_pickup = false;
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
                if (event.jbutton.button == option_joy_button[KEY_PICKUP])
                    action_pickup = true;
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
                if (event.jbutton.button == option_joy_button[KEY_PICKUP])
                    action_pickup = false;
                if (event.jbutton.button == option_joy_button[KEY_ACCEPT])
                    action_accept = false;
                if (event.jbutton.button == option_joy_button[KEY_PAUSE])
                    action_pause = false;
                if (event.jbutton.button == option_joy_button[KEY_EXIT])
                    action_exit = false;
            }
        }
        else if (joy && event.type == SDL_JOYAXISMOTION) {
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
}

void sysInputReset() {
    action_move = ACTION_NONE;
    action_switch = false;
    action_bump = false;
    action_pickup = false;
    action_accept = false;
    action_pause = false;
    action_exit = false;
}

void sysConfigSetPaths() {
#ifdef __ANDROID__
    String_Init(&path_dir_config, SDL_AndroidGetInternalStoragePath(), 0);
#elif _MSC_VER
    String_Init(&path_dir_config, getenv("AppData"), "/freeblocks", 0);
#elif __EMSCRIPTEN__
    String_Init(&path_dir_config, "/persistent_data", 0);
#else
    if (getenv("XDG_CONFIG_HOME"))
        String_Init(&path_dir_config, getenv("XDG_CONFIG_HOME"), "/freeblocks", 0);
    else
        String_Init(&path_dir_config, getenv("HOME"), "/.config/freeblocks", 0);
#endif

    String_Init(&path_file_config, path_dir_config.buf, "/config", 0);
    String_Init(&path_file_highscores, path_dir_config.buf, "/highscores", 0);
    String_Init(&path_file_highscores_jewels, path_dir_config.buf, "/highscores_jewels", 0);
    String_Init(&path_file_highscores_drop, path_dir_config.buf, "/highscores_drop", 0);
}

void sysConfigLoad() {
    char buffer[BUFSIZ];
    char *key = NULL;
    char *temp = NULL;
    bool version_match = false;

    mkdir(path_dir_config.buf, MKDIR_MODE);

    FILE *config_file = fopen(path_file_config.buf,"r+");
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
            else if (strcmp(key,"key_pickup") == 0) option_key[KEY_PICKUP] = (SDL_Keycode)atoi(strtok(NULL,"\n"));
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
    mkdir(path_dir_config.buf, MKDIR_MODE);

    FILE *config_file = fopen(path_file_config.buf,"w+");
    if (config_file) {
        fprintf(config_file,"version=%s\n", VERSION);

        fprintf(config_file,"joystick=%d\n",option_joystick);
        fprintf(config_file,"sound=%d\n",option_sound);
        fprintf(config_file,"music=%d\n",option_music);
        fprintf(config_file,"fullscreen=%d\n",option_fullscreen);

        fprintf(config_file,"\n# keyboard/GCW-Zero bindings\n");
        fprintf(config_file,"key_switch=%d\n",(int)option_key[KEY_SWITCH]);
        fprintf(config_file,"key_bump=%d\n",(int)option_key[KEY_BUMP]);
        fprintf(config_file,"key_pickup=%d\n",(int)option_key[KEY_PICKUP]);
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
#ifdef __EMSCRIPTEN__
        emscriptenWriteFile(path_file_config.buf);
#endif
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

    mkdir(path_dir_config.buf, MKDIR_MODE);

    file = fopen(game_mode->highscores->buf,"r+");

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

    mkdir(path_dir_config.buf, MKDIR_MODE);

    file = fopen(game_mode->highscores->buf,"w+");

    if (file) {
        for (i=0;i<10;i++) {
            fprintf(file,"%d\n",high_scores[i]);
        }
        fclose(file);

#ifdef __EMSCRIPTEN__
        emscriptenWriteFile(game_mode->highscores->buf);
#endif
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

#ifndef __EMSCRIPTEN__
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, args);
#else
    vprintf(format, args);
#endif

	va_end(args);
}

void logError(const char* format, ...) {
	va_list args;

	va_start(args, format);

#ifndef __EMSCRIPTEN__
	SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, format, args);
#else
    vprintf(format, args);
#endif

	va_end(args);
}


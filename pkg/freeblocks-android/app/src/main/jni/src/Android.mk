LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -std=c99

LOCAL_MODULE := main

SDL_PATH := ../SDL2

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
					$(LOCAL_PATH)/../SDL2_image \
					$(LOCAL_PATH)/../SDL2_mixer \
					$(LOCAL_PATH)/../SDL2_ttf

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	../../../../../../../src/dork/dork_string.c \
	../../../../../../../src/block.c \
	../../../../../../../src/draw.c \
	../../../../../../../src/easing.c \
	../../../../../../../src/game.c \
	../../../../../../../src/game_mode.c \
	../../../../../../../src/menu.c \
	../../../../../../../src/sys.c \
	../../../../../../../src/main.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer SDL2_ttf

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)

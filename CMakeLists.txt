Project (freeblocks)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "FREEBLOCKS")
Set (VERSION "0.5")

option(HALF_GFX "Use 320x240 graphics" Off)
if (HALF_GFX)
    add_definitions(-DHALF_GFX)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CROSSCOMPILING)
    # check for gcw0
    if(CMAKE_C_COMPILER MATCHES ".*gcw0-linux.*")
        add_definitions(-D__GCW0__)
    endif()
endif()

# Default definitions
if(MSVC)
    add_definitions(-Wall -W4 -wd"4996" -D_USE_MATH_DEFINES)
else()
    add_definitions(-Wall -Wextra -Wunused -Wshadow -Wunreachable-code -std=c99)
    add_definitions(-fno-math-errno -fno-exceptions)
    add_definitions(-Wno-unused-parameter)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -pg")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-pg")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "-pg")
endif()

set(BINDIR  "bin"                     CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game executable will be installed.")
set(DATADIR "share/games/freeblocks"  CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where game data files will be installed.")
set(APPDIR "share/applications"       CACHE STRING "Directory from CMAKE_INSTALL_PREFIX where the .desktop file will be installed.")

If(NOT IS_ABSOLUTE "${BINDIR}")
    set(FREEBLOCKS_EXECUTABLE_PATH ${CMAKE_INSTALL_PREFIX}/${BINDIR}/freeblocks)
Else(NOT IS_ABSOLUTE "${BINDIR}")
    set(FREEBLOCKS_EXECUTABLE_PATH ${BINDIR}/freeblocks)
EndIf(NOT IS_ABSOLUTE "${BINDIR}")

If(NOT IS_ABSOLUTE "${DATADIR}")
    add_definitions(-DPKGDATADIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}/res")
Else(NOT IS_ABSOLUTE "${DATADIR}")
    add_definitions(-DPKGDATADIR="${DATADIR}/res")
EndIf(NOT IS_ABSOLUTE "${DATADIR}")

If(NOT IS_ABSOLUTE "${APPDIR}")
    set(APPDIR ${CMAKE_INSTALL_PREFIX}/${APPDIR})
EndIf(NOT IS_ABSOLUTE "${APPDIR}")


# Check for SDL2
Find_Package(SDL2)
If (NOT SDL2_FOUND)
	Message (FATAL_ERROR "Couldn't find SDL2 development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-dev' package.")
Else (NOT SDL2_FOUND)
	INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
EndIf (NOT SDL2_FOUND)

Find_Package(SDL2_image)
If (NOT SDL2IMAGE_FOUND)
	Message (FATAL_ERROR "Couldn't find SDL2-image development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-image-dev' package.")
Else (NOT SDL2IMAGE_FOUND)
	INCLUDE_DIRECTORIES(${SDL2IMAGE_INCLUDE_DIR})
EndIf (NOT SDL2IMAGE_FOUND)

Find_Package(SDL2_mixer)
If (NOT SDL2MIXER_FOUND)
	Message (FATAL_ERROR "Couldn't find SDL2-mixer development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-mixer-dev' package.")
Else (NOT SDL2MIXER_FOUND)
	INCLUDE_DIRECTORIES(${SDL2MIXER_INCLUDE_DIR})
EndIf (NOT SDL2MIXER_FOUND)

Find_Package(SDL2_ttf)
If (NOT SDL2TTF_FOUND)
	Message (FATAL_ERROR "Couldn't find SDL2-ttf development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl2-ttf-dev' package.")
Else (NOT SDL2TTF_FOUND)
	INCLUDE_DIRECTORIES(${SDL2TTF_INCLUDE_DIR})
EndIf (NOT SDL2TTF_FOUND)


# Sources

Set (FREEBLOCKS_SOURCES
    ./src/dork/dork_string.c
    ./src/block.c
    ./src/draw.c
    ./src/easing.c
    ./src/game.c
    ./src/game_mode.c
    ./src/menu.c
    ./src/sys.c
    ./src/main.c
)

Set (FREEBLOCKS_HEADERS
    ./src/dork/dork_string.h
    ./src/block.h
    ./src/draw.h
    ./src/easing.h
    ./src/game.h
    ./src/game_mode.h
    ./src/menu.h
    ./src/sys.h
)

if(APPLE)
	set(FREEBLOCKS_EXTRA ./pkg/osx/SDLMain.m ./pkg/osx/SDLMain.h)
	set_source_files_properties(./pkg/osx/SDLMain.m PROPERTIES LANGUAGE C)
	set(EXTRA_LIBRARIES "-framework Carbon -framework IOKit")
endif()

Add_Executable (freeblocks ${FREEBLOCKS_SOURCES} ${FREEBLOCKS_HEADERS} ${FREEBLOCKS_EXTRA})

# libSDLMain comes with libSDL if needed on certain platforms
If (NOT SDL2MAIN_LIBRARY)
  Set (SDL2MAIN_LIBRARY "")
EndIf (NOT SDL2MAIN_LIBRARY)

If (UNIX)
    set(CMAKE_LD_FLAGS ${CMAKE_LD_FLAGS} m)
EndIf()

Target_Link_Libraries (freeblocks ${CMAKE_LD_FLAGS} ${SDL2_LIBRARY} ${SDL2IMAGE_LIBRARY} ${SDL2MIXER_LIBRARY} ${SDL2TTF_LIBRARY} ${SDL2MAIN_LIBRARY} ${EXTRA_LIBRARIES})

# installing to the proper places
install(TARGETS freeblocks DESTINATION ${BINDIR})
install(DIRECTORY res DESTINATION ${DATADIR})
install(FILES pkg/default.gcw0.desktop DESTINATION ${APPDIR} RENAME freeblocks.desktop)
install(FILES pkg/freeblocks.png DESTINATION share/icons/hicolor/32x32/apps)

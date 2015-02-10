Project (freeblocks)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "FREEBLOCKS")
Set (VERSION "0.4")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CROSSCOMPILING)
    # check for gcw0
    if(CMAKE_C_COMPILER MATCHES ".*gcw0-linux.*")
        add_definitions(-D__GCW0__)
    endif()
endif()

# Default definitions
if(MSVC)
    add_definitions(-Wall -W4 -wd"4996")
else()
    add_definitions(-Wall -Wextra -Wunused -Wshadow -Wunreachable-code -std=c99)
    add_definitions(-fno-math-errno -fno-exceptions)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    set(CMAKE_C_FLAGS_MINSIZEREL "-Os -g0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
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


# Check for SDL
Find_Package (SDL REQUIRED)
If (NOT SDL_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl1.2-dev' package.")
Else (NOT SDL_FOUND)
  Include_Directories (${SDL_INCLUDE_DIR})
EndIf (NOT SDL_FOUND)

Find_Package (SDL_mixer REQUIRED)
If (NOT SDLMIXER_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-mixer development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-mixer1.2-dev' package.")
Else (NOT SDLMIXER_FOUND)
  Include_Directories (${SDLMIXER_INCLUDE_DIR})
EndIf (NOT SDLMIXER_FOUND)

Find_Package (SDL_image REQUIRED)
If (NOT SDLIMAGE_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-image development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-image1.2-dev' package.")
Else (NOT SDLIMAGE_FOUND)
  Include_Directories (${SDLIMAGE_INCLUDE_DIR})
EndIf (NOT SDLIMAGE_FOUND)

Find_Package (SDL_ttf REQUIRED)
If (NOT SDLTTF_FOUND)
  Message (FATAL_ERROR "Couldn't find SDL-ttf development files. On Debian-based systems (such as Ubuntu) you should install the 'libsdl-ttf2.0-dev' package.")
Else (NOT SDLTTF_FOUND)
  Include_Directories (${SDLTTF_INCLUDE_DIR})
EndIf (NOT SDLTTF_FOUND)

# Sources

Set (FREEBLOCKS_SOURCES
    ./src/dork/dork_string.c
    ./src/block.c
    ./src/draw.c
    ./src/game.c
    ./src/menu.c
    ./src/sys.c
    ./src/main.c
)

Set (FREEBLOCKS_HEADERS
    ./src/dork/dork_string.h
    ./src/block.h
    ./src/draw.h
    ./src/game.h
    ./src/menu.h
    ./src/sys.h
)

Add_Executable (freeblocks ${FREEBLOCKS_SOURCES} ${FREEBLOCKS_HEADERS})

# libSDLMain comes with libSDL if needed on certain platforms
If (NOT SDLMAIN_LIBRARY)
  Set (SDLMAIN_LIBRARY "")
EndIf (NOT SDLMAIN_LIBRARY)

Target_Link_Libraries (freeblocks ${CMAKE_LD_FLAGS} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLTTF_LIBRARY} ${SDLMAIN_LIBRARY})

# installing to the proper places
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/freeblocks DESTINATION ${BINDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION ${DATADIR})
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pkg/default.gcw0.desktop" DESTINATION ${APPDIR} RENAME freeblocks.desktop)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/pkg/freeblocks.png" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/32x32/apps)

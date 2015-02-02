Project (freeblocks)
cmake_minimum_required (VERSION 2.6)

Set (PACKAGE "FREEBLOCKS")
Set (VERSION "0.1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Default definitions
set(CMAKE_C_FLAGS "-Wall -Wextra -Wunused -Wshadow -Wunreachable-code -std=c99 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS "-fno-math-errno -fno-exceptions ${CMAKE_C_FLAGS}")

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
    add_definitions(-DDATA_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}/${DATADIR}")
Else(NOT IS_ABSOLUTE "${DATADIR}")
    add_definitions(-DDATA_INSTALL_DIR="${DATADIR}")
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
    ./src/block.c
    ./src/draw.c
    ./src/game.c
    ./src/menu.c
    ./src/sys.c
    ./src/main.c
)

Set (FREEBLOCKS_HEADERS
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
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/freeblocks.desktop" DESTINATION ${APPDIR})

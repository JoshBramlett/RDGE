cmake_minimum_required (VERSION 3.4)
project (pong)

#
# Versioning
#
set (pong_VERSION_MAJOR 0)
set (pong_VERSION_MINOR 1)

message (STATUS "== Configuring example: pong ==")

set (BIN_DIR ${pong_SOURCE_DIR}/bin)
set (CMAKE_MODULE_PATH ${pong_SOURCE_DIR}/cmake)

if (
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang"
   )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++14")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
endif ()

find_package (SDL2 REQUIRED)
find_package (SDL2_image REQUIRED)
find_package (SDL2_ttf REQUIRED)

link_directories (${RDGE_SOURCE_DIR}/build/lib)

include_directories (${SDL2_INCLUDE_DIR})
include_directories (${SDL2_IMAGE_INCLUDE_DIR})
include_directories (${SDL2_TTF_INCLUDE_DIR})
include_directories ("${RDGE_SOURCE_DIR}/include")

add_executable (
                pong
                src/main.cpp
                src/game.cpp
                src/scenes/intro/introscene.cpp
                src/scenes/menu/menuscene.cpp
                src/scenes/settings/settingsscene.cpp
                src/scenes/pong/pongscene.cpp
                src/scenes/pong/entities/ball.cpp
                src/scenes/pong/entities/net.cpp
                src/scenes/pong/entities/paddles.cpp
                src/scenes/pong/entities/scoreboard.cpp
               )

target_link_libraries (
                       pong
                       ${SDL2_LIBRARY}
                       ${SDL2_IMAGE_LIBRARY}
                       ${SDL2_TTF_LIBRARY}
                       RDGE
                      )

install(TARGETS pong RUNTIME DESTINATION ${BIN_DIR})

cmake_minimum_required (VERSION 3.4)
project (gui)

message (STATUS "== Configuring example: gui ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

set (BIN_DIR ${gui_SOURCE_DIR}/bin)
set (CMAKE_MODULE_PATH ${gui_SOURCE_DIR}/cmake)

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

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories ("${RDGE_SOURCE_DIR}/include")

add_executable (
                gui
                src/main.cpp
                src/gui.cpp
                src/scenes/guiscene.cpp
               )

target_link_libraries (
                       gui
                       RDGE
                      )

add_custom_command(TARGET gui PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:gui>)

# TODO : Implement install
#install(TARGETS ex01_triangle RUNTIME DESTINATION ${BIN_DIR})

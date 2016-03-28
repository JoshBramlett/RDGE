cmake_minimum_required (VERSION 3.4)
project (RDGE)

set (RDGE_VERSION_MAJOR 0)
set (RDGE_VERSION_MINOR 0)
set (RDGE_VERSION_PACKAGE 2)

set (CMAKE_MODULE_PATH ${RDGE_SOURCE_DIR}/cmake)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# output
message (STATUS "RDGE v${RDGE_VERSION_MAJOR}.${RDGE_VERSION_MINOR}.${RDGE_VERSION_PACKAGE}")
message (STATUS "System:    ${CMAKE_SYSTEM_NAME}")
message (STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "Compiler:  ${CMAKE_CXX_COMPILER_ID}")

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

# google test framework
enable_testing ()
find_package (GTest REQUIRED)
include_directories (${GTEST_INCLUDE_DIRS})
add_executable (rdge_test tests/rdge_math.cpp)
target_link_libraries (rdge_test ${GTEST_BOTH_LIBRARIES})
#add_test(RDGE_UtilTest rdge_test)

find_package (SDL2 REQUIRED)
find_package (SDL2_image REQUIRED)
find_package (SDL2_ttf REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

include_directories (${SDL2_INCLUDE_DIR})
include_directories (${SDL2_IMAGE_INCLUDE_DIR})
include_directories (${SDL2_TTF_INCLUDE_DIR})
include_directories (${GLEW_INCLUDE_DIRS})
include_directories (${OPENGL_INCLUDE_DIR})

# project includes
include_directories (include)
include_directories (internal)

add_library (
             RDGE SHARED
             src/application.cpp
             src/window.cpp
             src/color.cpp
             src/font.cpp
             src/random.cpp
             src/surface.cpp
             src/texture.cpp
             src/cursor.cpp
             src/viewport.cpp
             src/controls/control.cpp
             src/controls/button.cpp
             src/gameobjects/icollidable.cpp
             src/gameobjects/ientity.cpp
             src/gameobjects/scene.cpp
             src/gameobjects/game.cpp
             src/gameobjects/text.cpp
             src/math/mat4.cpp
             src/math/vec2.cpp
             src/math/vec3.cpp
             src/math/vec4.cpp
             src/util/exception.cpp
             src/util/logger.cpp
             src/util/timer.cpp
            )

target_link_libraries (
                       RDGE
                       ${SDL2_LIBRARY}
                       ${SDL2_IMAGE_LIBRARY}
                       ${SDL2_TTF_LIBRARY}
                       ${GLEW_LIBRARIES}
                       ${OPENGL_LIBRARIES}
                      )

#add_subdirectory (examples/pong)



#target_include_directories (RDGE PUBLIC ${LIB_DIR})

# currently unused - both headers and libs should be moved to e.g. /usr/local/
#install (
         #TARGETS RDGE
         #DESTINATION ${LIB_DIR}
        #)

cmake_minimum_required (VERSION 3.4)
project (01_hello_rect)

message (STATUS "== Configuring tutorial: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    # Matches for "Clang" and "AppleClang"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++14")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++14")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
        string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
endif ()

find_package (SDL2 REQUIRED)
find_package (GLEW REQUIRED)
#find_package (OpenGL REQUIRED)


include_directories (${SDL2_INCLUDE_DIR})
#include_directories (${GLEW_INCLUDE_DIRS})
include_directories (${OPENGL_INCLUDE_DIR})


link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (01_hello_rect
                src/main.cpp)

target_link_libraries (ex01_triangle
                       ${SDL2_LIBRARY}
                       #${GLEW_LIBRARIES}
                       ${OPENGL_LIBRARIES}
                       RDGE)

#add_custom_command(TARGET ex01_triangle PRE_BUILD
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory
                   #${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:ex01_triangle>)

# TODO : Implement install
#install(TARGETS ex01_triangle RUNTIME DESTINATION ${BIN_DIR})

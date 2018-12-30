cmake_minimum_required (VERSION 3.4)
project (RDGE)

set (RDGE_VERSION_MAJOR 0)
set (RDGE_VERSION_MINOR 0)
set (RDGE_VERSION_PACKAGE 10)
set (RDGE_VERSION_STRING "${RDGE_VERSION_MAJOR}.${RDGE_VERSION_MINOR}.${RDGE_VERSION_PACKAGE}")
string(TIMESTAMP TODAY "%m/%d/%Y")

set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# Add CMake vars used in child projects
# Eventually these should be defined in a module lookup, like SDL, GLEW, or OpenGL
#set (RDGE_LINK_DIR ${RDGE_SOURCE_DIR}/lib)

# Echo build information
message ("Building RDGE v${RDGE_VERSION_STRING}")
message (STATUS "System:      ${CMAKE_SYSTEM_NAME} v${CMAKE_SYSTEM_VERSION}")
message (STATUS "Processor:   ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID}")
message (STATUS "Build type:  ${CMAKE_BUILD_TYPE}")

# ------------------------------------------------------------- #
#                      Header/Source Files                      #
# ------------------------------------------------------------- #

set (RDGE_INCLUDE_DIR ${RDGE_SOURCE_DIR}/include)
set (RDGE_INTERNAL_INCLUDE_DIR ${RDGE_SOURCE_DIR}/internal)

# General
set(RDGE_HEADER_FILES
    ${RDGE_INCLUDE_DIR}/rdge/application.hpp
    ${RDGE_INCLUDE_DIR}/rdge/core.hpp
    ${RDGE_INCLUDE_DIR}/rdge/type_traits.hpp)

set(RDGE_SOURCE_FILES
    ${RDGE_SOURCE_DIR}/src/application.cpp)

# Assets
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/assets.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/bitmap_font.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/pack_file.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/shared_asset.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/spritesheet.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/surface.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/tileset.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/file_formats/asset_pack.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/file_formats/bmfont.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/tilemap/layer.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/tilemap/object.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/tilemap/property.hpp
     ${RDGE_INCLUDE_DIR}/rdge/assets/tilemap/tilemap.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/assets/file_formats/asset_pack.cpp
     ${RDGE_SOURCE_DIR}/src/assets/file_formats/bmfont.cpp
     ${RDGE_SOURCE_DIR}/src/assets/file_formats/tiled.cpp
     ${RDGE_SOURCE_DIR}/src/assets/tilemap/layer.cpp
     ${RDGE_SOURCE_DIR}/src/assets/tilemap/object.cpp
     ${RDGE_SOURCE_DIR}/src/assets/tilemap/property.cpp
     ${RDGE_SOURCE_DIR}/src/assets/tilemap/tilemap.cpp
     ${RDGE_SOURCE_DIR}/src/assets/pack_file.cpp
     ${RDGE_SOURCE_DIR}/src/assets/bitmap_font.cpp
     ${RDGE_SOURCE_DIR}/src/assets/tileset.cpp
     ${RDGE_SOURCE_DIR}/src/assets/spritesheet.cpp
     ${RDGE_SOURCE_DIR}/src/assets/surface.cpp)

 # Debug
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/debug.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/assert.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/memory.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/renderer.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/sdl_dumper.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/camera_widget.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/graphics_widget.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/iwidget.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/memory_widget.hpp
     ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/physics_widget.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/debug/widgets/camera_widget.cpp
     ${RDGE_SOURCE_DIR}/src/debug/widgets/memory_widget.cpp
     ${RDGE_SOURCE_DIR}/src/debug/widgets/graphics_widget.cpp
     ${RDGE_SOURCE_DIR}/src/debug/widgets/physics_widget.cpp
     ${RDGE_SOURCE_DIR}/src/debug/renderer.cpp)

 # Events
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/events/event.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/events/event.cpp)

 # Game Objects
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects/input/ihandler.hpp
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects/input/keyboard_directional_input_handler.hpp
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects.hpp
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects/game.hpp
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects/iscene.hpp
     ${RDGE_INCLUDE_DIR}/rdge/gameobjects/types.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/gameobjects/input/keyboard_directional_input_handler.cpp
     ${RDGE_SOURCE_DIR}/src/gameobjects/game.cpp
     ${RDGE_SOURCE_DIR}/src/gameobjects/types.cpp)

# Graphics
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/graphics.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/animation.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/bitmap_charset.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/blend.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/color.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/orthographic_camera.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/tex_coords.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/texture.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/layers/sprite_layer.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/layers/tile_layer.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/renderers/sprite_batch.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/renderers/tile_batch.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/shaders/shader_program.hpp
     ${RDGE_INCLUDE_DIR}/rdge/graphics/shaders/sprite_batch_shader.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/graphics/layers/sprite_layer.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/layers/tile_layer.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/shaders/shader_program.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/shaders/sprite_batch_shader.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/renderers/sprite_batch.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/renderers/tile_batch.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/animation.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/bitmap_charset.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/blend.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/color.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/orthographic_camera.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/tex_coords.cpp
     ${RDGE_SOURCE_DIR}/src/graphics/texture.cpp)

 # Math
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/math.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/intrinsics.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/mat2.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/mat3.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/mat4.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/random.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/vec2.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/vec3.hpp
     ${RDGE_INCLUDE_DIR}/rdge/math/vec4.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/math/random.cpp
     ${RDGE_SOURCE_DIR}/src/math/mat2.cpp
     ${RDGE_SOURCE_DIR}/src/math/mat3.cpp
     ${RDGE_SOURCE_DIR}/src/math/mat4.cpp
     ${RDGE_SOURCE_DIR}/src/math/vec3.cpp
     ${RDGE_SOURCE_DIR}/src/math/vec4.cpp)

 # Physics
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/physics.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/aabb.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/bvh.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/collision.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/collision_graph.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/contact.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/fixture.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/isometry.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/rigid_body.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/solver.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/joints/base_joint.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/joints/revolute_joint.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/shapes/ishape.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/shapes/circle.hpp
     ${RDGE_INCLUDE_DIR}/rdge/physics/shapes/polygon.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/physics/joints/revolute_joint.cpp
     ${RDGE_SOURCE_DIR}/src/physics/shapes/ishape.cpp
     ${RDGE_SOURCE_DIR}/src/physics/shapes/circle.cpp
     ${RDGE_SOURCE_DIR}/src/physics/shapes/polygon.cpp
     ${RDGE_SOURCE_DIR}/src/physics/aabb.cpp
     ${RDGE_SOURCE_DIR}/src/physics/bvh.cpp
     ${RDGE_SOURCE_DIR}/src/physics/collision.cpp
     ${RDGE_SOURCE_DIR}/src/physics/collision_graph.cpp
     ${RDGE_SOURCE_DIR}/src/physics/contact.cpp
     ${RDGE_SOURCE_DIR}/src/physics/fixture.cpp
     ${RDGE_SOURCE_DIR}/src/physics/rigid_body.cpp
     ${RDGE_SOURCE_DIR}/src/physics/solver.cpp)

 # System
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/system.hpp
     ${RDGE_INCLUDE_DIR}/rdge/system/keyboard.hpp
     ${RDGE_INCLUDE_DIR}/rdge/system/mouse.hpp
     ${RDGE_INCLUDE_DIR}/rdge/system/types.hpp
     ${RDGE_INCLUDE_DIR}/rdge/system/window.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/system/keyboard.cpp
     ${RDGE_SOURCE_DIR}/src/system/mouse.cpp
     ${RDGE_SOURCE_DIR}/src/system/types.cpp
     ${RDGE_SOURCE_DIR}/src/system/window.cpp)

 # Util
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INCLUDE_DIR}/rdge/util.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/compiler.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/exception.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/json.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/logger.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/profiling.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/strings.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/timer.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/disruptor.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/enum_array.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/freelist.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/intrusive_list.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/iterators.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/stack_array.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/containers/threadsafe_queue.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/io/rwops_base.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/memory/alloc.hpp
     ${RDGE_INCLUDE_DIR}/rdge/util/memory/small_block_allocator.hpp)

list(APPEND RDGE_SOURCE_FILES
     ${RDGE_SOURCE_DIR}/src/util/io/rwops_base.cpp
     ${RDGE_SOURCE_DIR}/src/util/memory/alloc.cpp
     ${RDGE_SOURCE_DIR}/src/util/memory/small_block_allocator.cpp
     ${RDGE_SOURCE_DIR}/src/util/exception.cpp
     ${RDGE_SOURCE_DIR}/src/util/logger.cpp
     ${RDGE_SOURCE_DIR}/src/util/timer.cpp)

 # Internal
list(APPEND RDGE_HEADER_FILES
     ${RDGE_INTERNAL_INCLUDE_DIR}/rdge/internal/hints.hpp
     ${RDGE_INTERNAL_INCLUDE_DIR}/rdge/internal/opengl_wrapper.hpp)


# ------------------------------------------------------------- #
#                        Find dependencies                      #
# ------------------------------------------------------------- #

message ("Searching for dependencies...")
set (CMAKE_MODULE_PATH ${RDGE_SOURCE_DIR}/cmake)
include(FindPackageMessage)

find_package (Git)
if(GIT_FOUND)
    find_package_message(Git
                         "Found Package: Git v${GIT_VERSION_STRING}"
                         "[${GIT_EXECUTABLE}] [${GIT_VERSION_STRING}]")
else()
    message (FATAL_ERROR "Git not found")
endif()

set (SDL2_BUILDING_LIBRARY ON)
find_package (SDL2 CONFIG REQUIRED)
if(SDL2_FOUND)
    find_package_message(SDL2
                         "Found Package: SDL2 ${SDL2_LIBRARY}"
                         "[${SDL2_LIBRARY}] [${SDL2_INCLUDE_DIR}]")
else()
    message (FATAL_ERROR "SDL2 not found")
endif()

find_package (GLEW)
if(GLEW_FOUND)
    find_package_message(GLEW
                         "Found Package: GLEW ${GLEW_LIBRARIES}"
                         "[${GLEW_LIBRARIES}] [${GLEW_INCLUDE_DIRS}]")
else()
    message (FATAL_ERROR "GLEW not found")
endif()

find_package (OpenGL)
if(OPENGL_FOUND)
    find_package_message(OpenGL
                         "Found Package: OpenGL ${OPENGL_INCLUDE_DIR}"
                         "[${OPENGL_LIBRARIES}] [${OPENGL_INCLUDE_DIR}]")
else()
    message (FATAL_ERROR "OpenGL not found")
endif()

enable_testing()
include(CTest)
find_package (GTest MODULE REQUIRED)
if(GTEST_FOUND)
    find_package_message(GTest
                         "Found Package: GTest ${GTEST_BOTH_LIBRARIES}"
                         "[${GTEST_BOTH_LIBRARIES}] [${GTEST_INCLUDE_DIRS}]")
else()
    message (FATAL_ERROR "GTest not found")
endif()

# ------------------------------------------------------------- #
#                   Generate build hash/date                    #
# ------------------------------------------------------------- #

set (BUILD_HASH_COMMAND_ARGS "log" "-1" "--pretty=%H")
execute_process(COMMAND ${GIT_EXECUTABLE} ${BUILD_HASH_COMMAND_ARGS}
                RESULT_VARIABLE cmd_result
                OUTPUT_VARIABLE RDGE_COMMIT_HASH
                ERROR_VARIABLE cmd_error
                WORKING_DIRECTORY ${RDGE_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if (cmd_result)
    message (FATAL_ERROR "Build hash not found. error=${cmd_error}")
endif ()

set (BUILD_DATE_COMMAND_ARGS "log" "-1" "--format=%ad" "--date=local")
execute_process(COMMAND ${GIT_EXECUTABLE} ${BUILD_DATE_COMMAND_ARGS}
                RESULT_VARIABLE cmd_result
                OUTPUT_VARIABLE RDGE_COMMIT_DATE
                ERROR_VARIABLE cmd_error
                WORKING_DIRECTORY ${RDGE_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if (cmd_result)
    message (FATAL_ERROR "Build hash not found. error=${cmd_error}")
endif ()

set (BUILD_SUBJECT_COMMAND_ARGS "log" "-1" "--format=%s")
execute_process(COMMAND ${GIT_EXECUTABLE} ${BUILD_SUBJECT_COMMAND_ARGS}
                RESULT_VARIABLE cmd_result
                OUTPUT_VARIABLE RDGE_COMMIT_SUBJECT
                ERROR_VARIABLE cmd_error
                WORKING_DIRECTORY ${RDGE_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE)
if (cmd_result)
    message (FATAL_ERROR "Build hash not found. error=${cmd_error}")
endif ()

# ------------------------------------------------------------- #
#                   Create versioning header                    #
# ------------------------------------------------------------- #

set (VERSION_FILE_IN "${CMAKE_MODULE_PATH}/version.hpp.tmpl")
set (VERSION_FILE_OUT "${RDGE_INTERNAL_INCLUDE_DIR}/rdge/internal/version.hpp")
configure_file("${CMAKE_MODULE_PATH}/version.hpp.tmpl"
               "${RDGE_INTERNAL_INCLUDE_DIR}/rdge/internal/version.hpp" @ONLY)
list(APPEND RDGE_HEADER_FILES ${VERSION_FILE_OUT})

message ("Version Control")
message (STATUS "Build hash:     ${RDGE_COMMIT_HASH}")
message (STATUS "Build date:     ${RDGE_COMMIT_DATE}")
message (STATUS "Build subject:  ${RDGE_COMMIT_SUBJECT}")
message (STATUS "Generated version header:  ${VERSION_FILE_OUT}")





# Flags for AddressSanitizer
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")

# Add custom debug flag
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DRDGE_DEBUG")

# alternatively...
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_definitions("-DRDGE_DEBUG")
#endif ()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set compiler flags
if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # Matches for "Clang" and "AppleClang"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ASAN_FLAGS}")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ASAN_FLAGS}")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
        string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
endif ()

# google test framework
#enable_testing()
#include(CTest)


#include_directories (${GTEST_INCLUDE_DIRS}
#                     ${SDL2_INCLUDE_DIR}
#                     ${GLEW_INCLUDE_DIRS}
#                     ${OPENGL_INCLUDE_DIR})

set(imgui_SOURCE_FILES
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui.cpp
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui_widgets.cpp
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui_draw.cpp
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui_demo.cpp
    ${RDGE_SOURCE_DIR}/lib/imgui/imgui_impl_rdge.cpp)

add_library (imgui STATIC
             lib/imgui/imgui.cpp
             lib/imgui/imgui_widgets.cpp
             lib/imgui/imgui_draw.cpp
             lib/imgui/imgui_demo.cpp
             lib/imgui/imgui_impl_rdge.cpp)
target_link_libraries (imgui
                       PUBLIC
                       PRIVATE SDL2::SDL2
                       PRIVATE GLEW::GLEW)









add_library (RDGE STATIC
             ${RDGE_HEADER_FILES}
             ${RDGE_SOURCE_FILES})

# project includes


target_include_directories (RDGE SYSTEM PUBLIC ${RDGE_SOURCE_DIR}/lib)
target_include_directories (RDGE PUBLIC ${RDGE_INCLUDE_DIR})
target_include_directories (RDGE PRIVATE ${RDGE_SOURCE_DIR}/internal)

target_link_libraries (RDGE
                       PUBLIC
                       imgui
                       PUBLIC SDL2::SDL2
                       PUBLIC GLEW::GLEW
                       #SDL2::SDL2main
                       #${SDL2_LIBRARY}
                       #${GLEW_LIBRARIES}
                       ${OPENGL_LIBRARIES})

message ("Adding subdirectories...")
add_subdirectory (tools/asset_packer)
#add_subdirectory (tools/pyxel2tiled)
add_subdirectory (sandbox/chrono)
add_subdirectory (sandbox/physics)

add_executable (rdge_test
                tests/main.cpp
                tests/assets/tilemap/layer_test.cpp
                tests/assets/tilemap/object_test.cpp
                tests/assets/tilemap/property_test.cpp
                tests/assets/file_formats/bmfont_test.cpp
                tests/assets/spritesheet_test.cpp
                tests/graphics/color_test.cpp
                tests/physics/aabb_test.cpp
                tests/physics/gjk_test.cpp
                tests/physics/circle_test.cpp
                tests/physics/polygon_test.cpp
                tests/math/intrinsics_test.cpp
                tests/math/vec2_test.cpp
                tests/system/types_test.cpp
                tests/util/freelist_test.cpp
                tests/util/intrusive_list_test.cpp
                tests/util/intrusive_forward_list_test.cpp)

target_link_libraries (rdge_test
                       PUBLIC RDGE
                       #PRIVATE GTest::GTest GTest::Main)
                       ${GTEST_BOTH_LIBRARIES})
#target_include_directories (rdge_test SYSTEM PUBLIC ${RDGE_SOURCE_DIR}/lib)
#target_include_directories (rdge_test PUBLIC ${RDGE_INCLUDE_DIR})

add_test(Vec2FloatingPointTest rdge_test)
add_test(SpriteSheetTest rdge_test)
add_test(ScreenPointTest rdge_test)
add_test(ScreenRectTest rdge_test)





#target_include_directories (RDGE PUBLIC ${LIB_DIR})

# currently unused - both headers and libs should be moved to e.g. /usr/local/
#install (
         #TARGETS RDGE
         #DESTINATION ${LIB_DIR}
        #)

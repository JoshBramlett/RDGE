cmake_minimum_required (VERSION 3.4)
project (RDGE)

set (RDGE_VERSION_MAJOR 0)
set (RDGE_VERSION_MINOR 0)
set (RDGE_VERSION_PACKAGE 10)

set (CMAKE_MODULE_PATH ${RDGE_SOURCE_DIR}/cmake)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# Add CMake vars used in child projects
# Eventually these should be defined in a module lookup, like SDL, GLEW, or OpenGL
set (RDGE_INCLUDE_DIR ${RDGE_SOURCE_DIR}/include)
#set (RDGE_LINK_DIR ${RDGE_SOURCE_DIR}/lib)


# Echo build information
message (STATUS "RDGE v${RDGE_VERSION_MAJOR}.${RDGE_VERSION_MINOR}.${RDGE_VERSION_PACKAGE}")
message (STATUS "System:      ${CMAKE_SYSTEM_NAME} v${CMAKE_SYSTEM_VERSION}")
message (STATUS "Processor:   ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "Compiler:    ${CMAKE_CXX_COMPILER_ID}")
message (STATUS "Build type:  ${CMAKE_BUILD_TYPE}")

# Flags for AddressSanitizer
set(ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")

# Add custom debug flag
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DRDGE_DEBUG")

# alternatively...
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_definitions("-DRDGE_DEBUG")
#endif ()

# Set compiler flags
if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # Matches for "Clang" and "AppleClang"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++1z")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ASAN_FLAGS}")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++17")
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${ASAN_FLAGS}")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (${CMAKE_CXX_FLAGS} MATCHES "/W[0-4]")
        string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
endif ()

# google test framework
enable_testing ()
include(CTest)

find_package (GTest REQUIRED)
find_package (SDL2 REQUIRED)
find_package (GLEW REQUIRED)
find_package (OpenGL REQUIRED)

include_directories (${GTEST_INCLUDE_DIRS})
include_directories (${SDL2_INCLUDE_DIR})
include_directories (${GLEW_INCLUDE_DIRS})
include_directories (${OPENGL_INCLUDE_DIR})

add_library (imgui STATIC
             lib/imgui/imgui.cpp
             lib/imgui/imgui_widgets.cpp
             lib/imgui/imgui_draw.cpp
             lib/imgui/imgui_demo.cpp
             lib/imgui/imgui_impl_rdge.cpp)

target_link_libraries (imgui
                       ${SDL2_LIBRARY}
                       ${GLEW_LIBRARIES}
                       ${OPENGL_LIBRARIES})

set(RDGE_HEADER_FILES
    ${RDGE_INCLUDE_DIR}/rdge/application.hpp
    ${RDGE_INCLUDE_DIR}/rdge/core.hpp
    ${RDGE_INCLUDE_DIR}/rdge/type_traits.hpp

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
    ${RDGE_INCLUDE_DIR}/rdge/assets/tilemap/tilemap.hpp

    ${RDGE_INCLUDE_DIR}/rdge/debug.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/assert.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/memory.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/renderer.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/sdl_dumper.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/camera_widget.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/graphics_widget.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/iwidget.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/memory_widget.hpp
    ${RDGE_INCLUDE_DIR}/rdge/debug/widgets/physics_widget.hpp

    ${RDGE_INCLUDE_DIR}/rdge/gameobjects.hpp
    ${RDGE_INCLUDE_DIR}/rdge/gameobjects/game.hpp
    ${RDGE_INCLUDE_DIR}/rdge/gameobjects/iscene.hpp
    ${RDGE_INCLUDE_DIR}/rdge/gameobjects/types.hpp
    ${RDGE_INCLUDE_DIR}/rdge/gameobjects/input/ihandler.hpp
    ${RDGE_INCLUDE_DIR}/rdge/gameobjects/input/keyboard_directional_input_handler.hpp

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
    ${RDGE_INCLUDE_DIR}/rdge/graphics/shaders/sprite_batch_shader.hpp

    ${RDGE_INCLUDE_DIR}/rdge/math.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/intrinsics.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/mat2.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/mat3.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/mat4.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/random.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/vec2.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/vec3.hpp
    ${RDGE_INCLUDE_DIR}/rdge/math/vec4.hpp

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
    ${RDGE_INCLUDE_DIR}/rdge/physics/shapes/polygon.hpp

    ${RDGE_INCLUDE_DIR}/rdge/system.hpp
    ${RDGE_INCLUDE_DIR}/rdge/system/keyboard.hpp
    ${RDGE_INCLUDE_DIR}/rdge/system/mouse.hpp
    ${RDGE_INCLUDE_DIR}/rdge/system/types.hpp
    ${RDGE_INCLUDE_DIR}/rdge/system/window.hpp

    ${RDGE_INCLUDE_DIR}/rdge/util.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/compiler.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/exception.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/json.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/logger.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/profiling.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/strings.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/timer.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/disruptor.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/freelist.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/intrusive_list.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/iterators.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/stack_array.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/containers/threadsafe_queue.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/io/rwops_base.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/memory/alloc.hpp
    ${RDGE_INCLUDE_DIR}/rdge/util/memory/small_block_allocator.hpp
    )

add_library (RDGE SHARED
             ${RDGE_HEADER_FILES}
             src/application.cpp

             src/assets/file_formats/asset_pack.cpp
             src/assets/file_formats/bmfont.cpp
             src/assets/file_formats/tiled.cpp
             src/assets/tilemap/layer.cpp
             src/assets/tilemap/object.cpp
             src/assets/tilemap/property.cpp
             src/assets/tilemap/tilemap.cpp
             src/assets/pack_file.cpp
             src/assets/bitmap_font.cpp
             src/assets/tileset.cpp
             src/assets/spritesheet.cpp
             src/assets/surface.cpp

             src/debug/widgets/camera_widget.cpp
             src/debug/widgets/memory_widget.cpp
             src/debug/widgets/graphics_widget.cpp
             src/debug/widgets/physics_widget.cpp
             src/debug/renderer.cpp

             src/events/event.cpp

             src/gameobjects/input/keyboard_directional_input_handler.cpp
             src/gameobjects/game.cpp
             src/gameobjects/types.cpp

             src/graphics/layers/sprite_layer.cpp
             src/graphics/layers/tile_layer.cpp
             src/graphics/shaders/shader_program.cpp
             src/graphics/shaders/sprite_batch_shader.cpp
             src/graphics/renderers/sprite_batch.cpp
             src/graphics/renderers/tile_batch.cpp
             src/graphics/animation.cpp
             src/graphics/bitmap_charset.cpp
             src/graphics/blend.cpp
             src/graphics/color.cpp
             src/graphics/orthographic_camera.cpp
             src/graphics/tex_coords.cpp
             src/graphics/texture.cpp

             #src/controls/control.cpp --> src/graphics/ui/widget.cpp
             #src/controls/button.cpp  --> src/graphics/ui/button.cpp

             src/math/random.cpp
             src/math/mat2.cpp
             src/math/mat3.cpp
             src/math/mat4.cpp
             src/math/vec3.cpp
             src/math/vec4.cpp

             src/physics/joints/revolute_joint.cpp
             src/physics/shapes/ishape.cpp
             src/physics/shapes/circle.cpp
             src/physics/shapes/polygon.cpp
             src/physics/aabb.cpp
             src/physics/bvh.cpp
             src/physics/collision.cpp
             src/physics/collision_graph.cpp
             src/physics/contact.cpp
             src/physics/fixture.cpp
             src/physics/rigid_body.cpp
             src/physics/solver.cpp

             src/system/keyboard.cpp
             src/system/mouse.cpp
             src/system/window.cpp

             src/util/io/rwops_base.cpp
             src/util/memory/alloc.cpp
             src/util/memory/small_block_allocator.cpp
             src/util/exception.cpp
             src/util/logger.cpp
             src/util/timer.cpp)

# project includes
target_include_directories (RDGE SYSTEM PUBLIC ${RDGE_SOURCE_DIR}/lib)
target_include_directories (RDGE PUBLIC ${RDGE_INCLUDE_DIR})
target_include_directories (RDGE PRIVATE ${RDGE_SOURCE_DIR}/internal)

target_link_libraries (RDGE
                       imgui
                       ${SDL2_LIBRARY}
                       ${GLEW_LIBRARIES}
                       ${OPENGL_LIBRARIES})

add_subdirectory (tools/asset_packer)
add_subdirectory (tools/pyxel2tiled)
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
                tests/util/freelist_test.cpp
                tests/util/intrusive_list_test.cpp
                tests/util/intrusive_forward_list_test.cpp)

target_link_libraries (rdge_test
                       RDGE
                       ${GTEST_BOTH_LIBRARIES})
#add_test(RDGE_UtilTest rdge_test)





#target_include_directories (RDGE PUBLIC ${LIB_DIR})

# currently unused - both headers and libs should be moved to e.g. /usr/local/
#install (
         #TARGETS RDGE
         #DESTINATION ${LIB_DIR}
        #)

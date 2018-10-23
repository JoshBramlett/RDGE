cmake_minimum_required (VERSION 3.4)
project (chrono)

message (STATUS "== Configuring sandbox: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

# project includes
include_directories (include)

add_executable (chrono
                src/events.cpp
                src/import.cpp
                src/main.cpp
                src/entities/iactor.cpp
                src/entities/player.cpp
                src/entities/npcs/debutante.cpp
                src/entities/static_actor.cpp
                src/entities/sign.cpp
                src/scenes/contact_handler.cpp
                src/scenes/scene_manager.cpp
                src/scenes/overworld.cpp
                src/scenes/winery.cpp)

target_link_libraries (chrono
                       RDGE)

add_custom_command(TARGET chrono PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:chrono>/res/export)

# TODO : Disabling for asset packing
#add_custom_command(TARGET chrono PRE_BUILD
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory
                   #${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:chrono>/res)

# TODO : Implement install
#install(TARGETS chrono RUNTIME DESTINATION ${BIN_DIR})

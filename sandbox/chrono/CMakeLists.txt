cmake_minimum_required (VERSION 3.4)
project (chrono)

message (STATUS "== Configuring sandbox: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (chrono
                src/main.cpp
                src/scenes/test.cpp
                src/scenes/entities/duck.cpp
                src/scenes/entities/dove.cpp
                src/scenes/entities/player.cpp)

target_link_libraries (chrono
                       RDGE)

# TODO : Disabling for asset packing
#add_custom_command(TARGET chrono PRE_BUILD
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory
                   #${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:chrono>/res)

# TODO : Implement install
#install(TARGETS chrono RUNTIME DESTINATION ${BIN_DIR})

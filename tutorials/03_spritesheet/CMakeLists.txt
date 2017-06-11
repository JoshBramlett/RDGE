cmake_minimum_required (VERSION 3.4)
project (03_spritesheet)

message (STATUS "== Configuring tutorial: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (03_spritesheet
                src/main.cpp)

target_link_libraries (03_spritesheet
                       RDGE)

# copy resources from common to build
configure_file(../res_common/spritesheet_stall.json res/spritesheet_stall.json COPYONLY)
configure_file(../res_common/spritesheet_stall.png res/spritesheet_stall.png COPYONLY)
configure_file(../res_common/counter.png res/counter.png COPYONLY)

# TODO : Implement install
#install(TARGETS 03_spritesheet RUNTIME DESTINATION ${BIN_DIR})

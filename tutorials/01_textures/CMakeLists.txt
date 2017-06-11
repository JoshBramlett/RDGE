cmake_minimum_required (VERSION 3.4)
project (01_textures)

message (STATUS "== Configuring tutorial: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (01_textures
                src/main.cpp)

target_link_libraries (01_textures
                       RDGE)

# copy resources from common to build
configure_file(../res_common/OpenSansPX.ttf res/OpenSansPX.ttf COPYONLY)
configure_file(../res_common/duck.png res/duck.png COPYONLY)

# TODO : Implement install
#install(TARGETS 01_textures RUNTIME DESTINATION ${BIN_DIR})

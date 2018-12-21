cmake_minimum_required (VERSION 3.4)
project (asset_packer)

message (STATUS "== Configuring sandbox: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (asset_packer
                src/import_fonts.cpp
                src/import_images.cpp
                src/import_spritesheets.cpp
                src/import_tilemaps.cpp
                src/import_tilesets.cpp
                src/main.cpp)

set_target_properties(asset_packer PROPERTIES
                      CXX_STANDARD 14
                      CXX_STANDARD_REQUIRED YES
                      CXX_EXTENSIONS NO)

target_link_libraries (asset_packer
                       PUBLIC RDGE)

# TODO : Implement install
#install(TARGETS physics RUNTIME DESTINATION ${BIN_DIR})

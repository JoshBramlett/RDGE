cmake_minimum_required (VERSION 3.4)
project (physics)

message (STATUS "== Configuring sandbox: ${PROJECT_NAME} ==")
message (STATUS "RDGE lib dir: ${RDGE_BINARY_DIR}/lib")
message (STATUS "RDGE include dir: ${RDGE_SOURCE_DIR}/include")

# uses parent module path
#set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

link_directories (${RDGE_BINARY_DIR}/lib)
include_directories (${RDGE_INCLUDE_DIR})

add_executable (physics
                src/main.cpp
                src/scenes/test.cpp)

target_link_libraries (physics
                       RDGE)

#add_custom_command(TARGET physics PRE_BUILD
                   #COMMAND ${CMAKE_COMMAND} -E copy_directory
                   #${PROJECT_SOURCE_DIR}/res $<TARGET_FILE_DIR:physics>/res)

# TODO : Implement install
#install(TARGETS physics RUNTIME DESTINATION ${BIN_DIR})

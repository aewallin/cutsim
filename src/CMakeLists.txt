cmake_minimum_required(VERSION 2.8)
include_directories(.)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

add_subdirectory(cutsim)
add_subdirectory(g2m)
add_subdirectory(app)

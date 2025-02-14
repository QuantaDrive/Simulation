cmake_minimum_required(VERSION 3.30)
project(Simulation)

add_library(Simulation ${GLFW_LIBRARY_TYPE}
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Simulation/Simulation.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp")

target_include_directories(Simulation PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(Simulation PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")
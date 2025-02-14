cmake_minimum_required(VERSION 3.30)
project(Simulation)

add_library(${CMAKE_PROJECT_NAME} ${GLFW_LIBRARY_TYPE}
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Simulation/Simulation.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.cpp"
        src/Mesh.cpp
        src/Mesh.h)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")
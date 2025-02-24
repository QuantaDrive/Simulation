cmake_minimum_required(VERSION 3.30)
project(Simulation)

include_directories(
        ../../libs/glfw-3.4/include
        ../../libs/glew-1.13.0/include
        ../../libs/glm-0.9.6.3
)

### Simulation lib ###

add_library(Simulation STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include/Simulation/Simulation.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Init.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Visualization.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp"
        src/RobotArm.cpp
        src/RobotArm.h
        src/Utils.h

)

target_include_directories(Simulation PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(Simulation PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")

###

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libs/inifile-cpp-main")

target_link_libraries(Simulation
        OpenGL::GL
        GLEW_1130
        glfw
        inicpp::inicpp
)
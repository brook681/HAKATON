cmake_minimum_required(VERSION 3.20)

project(ThreeBodySimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SFML 3
find_package(SFML 3 CONFIG REQUIRED COMPONENTS Graphics Window System)

# Physics library
add_library(physics
    src/physics/Physics.cpp
    src/physics/Scenarios.cpp
)

target_include_directories(physics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
)

# Simulation library
add_library(simulation
    src/simulation/Simulation.cpp
)

target_include_directories(simulation PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
)

target_link_libraries(simulation PUBLIC physics)

# Graphics library
add_library(graphics
    src/graphics/Graphics.cpp
    src/graphics/Menu.cpp
    src/graphics/CoordinateSystem.h
    src/graphics/Menu.h
)

target_include_directories(graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(graphics PUBLIC
    SFML::Graphics
    SFML::Window
    SFML::System
)

# Main application
add_executable(ThreeBodySimulation
    main.cpp
)

target_include_directories(ThreeBodySimulation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ThreeBodySimulation PRIVATE
    simulation
    graphics
)

# Physics tests
add_executable(physics_tests
    tests/physics_tests.cpp
)

target_link_libraries(physics_tests PRIVATE
    physics
    simulation
)

# Simulation tests
add_executable(simulation_tests
    tests/simulation_tests.cpp
)

target_link_libraries(simulation_tests PRIVATE
    simulation
)

# Compiler warnings
if(MSVC)
    foreach(target physics simulation graphics ThreeBodySimulation physics_tests simulation_tests)
        target_compile_options(${target} PRIVATE /W4)
    endforeach()
else()
    foreach(target physics simulation graphics ThreeBodySimulation physics_tests simulation_tests)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endforeach()
endif()

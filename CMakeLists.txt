
cmake_minimum_required(VERSION 3.20)

project(ThreeBodyPhysics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Physics tests
add_executable(physics_tests
    tests/physics_tests.cpp
)

target_link_libraries(physics_tests PRIVATE physics)

# Simulation tests
add_executable(simulation_tests
    tests/simulation_tests.cpp
)

target_link_libraries(simulation_tests PRIVATE simulation)

# Compiler warnings
if(MSVC)
    target_compile_options(physics PRIVATE /W4)
    target_compile_options(simulation PRIVATE /W4)
    target_compile_options(physics_tests PRIVATE /W4)
    target_compile_options(simulation_tests PRIVATE /W4)
else()
    target_compile_options(physics PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(simulation PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(physics_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(simulation_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

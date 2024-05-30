cmake_minimum_required(VERSION 3.14)
project(odd_knapsack_problem VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(ortools CONFIG REQUIRED)

add_executable(odd_knapsack_problem odd_knapsack_problem.cpp read_input.h read_input.cpp)
target_link_libraries(odd_knapsack_problem ortools::ortools)

# Debug changes for ortools being installed from binaries
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    # Unset _DEBUG for debug builds
    remove_definitions("_D_DEBUG")
    # Set /MD since we link statically
    set(CMAKE_CXX_FLAGS_DEBUG "/MD")
    
    # Set _ITERATOR_DEBUG_LEVEL to 0 for debug builds
    add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=0>)
endif()

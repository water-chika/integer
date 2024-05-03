cmake_minimum_required(VERSION 3.20)

project(integer)

add_library(integer
    integer.hpp
    integer.cpp)
target_include_directories(integer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(integer PROPERTIES CXX_STANDARD 23)

add_executable(test_integer
    main.cpp)
target_link_libraries(test_integer PUBLIC integer)
set_target_properties(test_integer PROPERTIES CXX_STANDARD 23)

cmake_minimum_required(VERSION 3.20)

project(integer)

add_subdirectory(execute_binary_code)

add_library(integer
    integer.hpp
    integer.cpp)
target_include_directories(integer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(integer PUBLIC execute_binary_code)
target_compile_features(integer PUBLIC cxx_std_23)

add_executable(test_integer
    main.cpp)
target_link_libraries(test_integer PUBLIC integer)

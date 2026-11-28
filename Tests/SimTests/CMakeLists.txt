cmake_minimum_required(VERSION 3.16)
project(TerraForgeSimTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tf_sim_tests main.cpp)
target_include_directories(tf_sim_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/TerraForge/Public)

if(MSVC)
  target_compile_options(tf_sim_tests PRIVATE /W4 /WX /utf-8)
else()
  target_compile_options(tf_sim_tests PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wno-sign-conversion -Werror)
endif()

enable_testing()
add_test(NAME tf_sim_tests COMMAND tf_sim_tests)

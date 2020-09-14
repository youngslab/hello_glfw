

cmake_minimum_required(VERSION 3.10)

project(welcom)

add_subdirectory(thirdparty)

add_executable(${CMAKE_PROJECT_NAME} main.cpp )

target_link_libraries(${CMAKE_PROJECT_NAME} glfw)

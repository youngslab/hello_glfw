

cmake_minimum_required(VERSION 3.10)

project(hello-glfw)

add_subdirectory(thirdparty)

add_executable(${CMAKE_PROJECT_NAME} src/main.cpp )

target_link_libraries(${CMAKE_PROJECT_NAME} glfw GLESv2 EGL)

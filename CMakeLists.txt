cmake_minimum_required(VERSION 3.10)

# set the project name
project(OpenGL1)

# add the executable
add_executable(OpenGL1 main.cpp)

target_link_libraries(OpenGL1 -DCMAKE_TOOLCHAIN_FILE=C:/Users/prutm/vcpkg/scripts/buildsystems/vcpkg.cmake)

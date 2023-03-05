cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hw8 LANGUAGES CXX)

set(project_path "${PROJECT_SOURCE_DIR}/")
configure_file(config/config.h.in ../config.h)

find_package(libigl CONFIG REQUIRED)

add_executable(hw8 hw8.cpp App.cpp)

target_link_libraries(hw8 PRIVATE igl::core igl::common igl::opengl igl::opengl_glfw)
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hw6 LANGUAGES CXX)

add_definitions(-D_USE_MATH_DEFINES)

find_package(libigl CONFIG REQUIRED)
find_package(openmesh REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(hw6 main.cpp)
target_link_libraries(hw6 PRIVATE igl::core igl::common igl::opengl igl::opengl_glfw)
target_include_directories(hw6 PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(hw6 PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(hw6 PRIVATE fmt::fmt)

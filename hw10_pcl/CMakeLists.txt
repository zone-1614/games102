cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(project_path "${CMAKE_CURRENT_SOURCE_DIR}/")

configure_file(config.h.in "${project_path}config.h")

project(surface_reconstruction LANGUAGES CXX)

find_package(PCL CONFIG REQUIRED)

add_executable(poisson poisson.cpp)
add_executable(rbf rbf.cpp)

target_link_libraries(poisson PRIVATE ${PCL_LIBRARIES})
target_link_libraries(rbf PRIVATE ${PCL_LIBRARIES})

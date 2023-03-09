cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hw9 LANGUAGES CXX)

add_definitions(-D_USE_MATH_DEFINES)
set(model_path "${PROJECT_SOURCE_DIR}/model/")
configure_file(config/config.h.in ../config.h)

find_package(libigl CONFIG REQUIRED)
find_package(openmesh REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(hw9 hw9.cpp QEM.cpp)

target_link_libraries(hw9 PRIVATE igl::core igl::common igl::opengl igl::opengl_glfw)
target_include_directories(hw9 PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(hw9 PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(hw9 PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(hw7 LANGUAGES CXX)

set(model_path "${PROJECT_SOURCE_DIR}/model/")
configure_file("${PROJECT_SOURCE_DIR}/config/config.h.in" "${PROJECT_SOURCE_DIR}/config.h")

add_definitions(-D_USE_MATH_DEFINES)

find_package(libigl CONFIG REQUIRED)
find_package(openmesh REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(hw7 hw7.cpp App.cpp)
target_link_libraries(hw7 PRIVATE igl::core igl::common igl::opengl igl::opengl_glfw)
target_include_directories(hw7 PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(hw7 PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(hw7 PRIVATE Eigen3::Eigen)
target_link_libraries(hw7 PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

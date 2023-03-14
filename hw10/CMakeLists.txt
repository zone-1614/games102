cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rbf LANGUAGES CXX)

add_definitions(-D_USE_MATH_DEFINES)

set(model_path "${PROJECT_SOURCE_DIR}/model/")
configure_file(config/config.h.in "${PROJECT_SOURCE_DIR}/include/rbf/config.h")

find_package(openmesh CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp include/rbf/*.h)
add_executable(rbf ${srcs})

target_include_directories(rbf PUBLIC include)
target_include_directories(rbf PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(rbf PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(rbf PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

enable_testing()
add_subdirectory(test)

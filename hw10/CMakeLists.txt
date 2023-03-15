cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rbf LANGUAGES CXX)

add_definitions(-D_USE_MATH_DEFINES)

# add_compile_options(/bigobj)

# option(DEBUG "debug" ON)
# if (DEFINED DEBUG)
#     add_definitions(-DDEBUG)
# endif()

set(model_path "${PROJECT_SOURCE_DIR}/model/")
configure_file(config/config.h.in "${PROJECT_SOURCE_DIR}/include/rbf/config.h")

find_package(openmesh CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp include/rbf/*.h)
add_executable(rbf ${srcs})

target_include_directories(rbf PUBLIC include)
target_include_directories(rbf PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(rbf PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(rbf PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(rbf PRIVATE Eigen3::Eigen)

enable_testing()
add_subdirectory(test)

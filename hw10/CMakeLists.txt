cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rbf_surface_reconstruction LANGUAGES CXX)

add_definitions(-D_USE_MATH_DEFINES)

add_compile_options(/bigobj)

set(model_path "${PROJECT_SOURCE_DIR}/model/")
set(log_path "${PROJECT_SOURCE_DIR}/log/")
configure_file(config/config.h.in "${PROJECT_SOURCE_DIR}/include/rbf/config.h")

find_package(openmesh CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
# file(GLOB srcs CONFIGURE_DEPENDS src/*.cpp include/rbf/*.h)
add_executable(rbf_fit src/RBF.cpp src/rbf_fit.cpp src/util.cpp src/kdtree.cpp)

target_include_directories(rbf_fit PUBLIC include)
target_include_directories(rbf_fit PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(rbf_fit PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(rbf_fit PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(rbf_fit PRIVATE Eigen3::Eigen)
target_link_libraries(rbf_fit PRIVATE nlohmann_json::nlohmann_json)

add_executable(rbf_fit_sparse src/RBF.cpp src/rbf_fit_sparse.cpp src/util.cpp src/kdtree.cpp)
target_include_directories(rbf_fit_sparse PUBLIC include)
target_include_directories(rbf_fit_sparse PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(rbf_fit_sparse PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(rbf_fit_sparse PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(rbf_fit_sparse PRIVATE Eigen3::Eigen)
target_link_libraries(rbf_fit_sparse PRIVATE nlohmann_json::nlohmann_json)

add_executable(march_iso src/march_iso.cpp src/MarchingCube.cpp src/util.cpp src/octree.cpp src/kdtree.cpp)
target_include_directories(march_iso PUBLIC include)
target_include_directories(march_iso PRIVATE ${OPENMESH_INCLUDE_DIRS})
target_link_libraries(march_iso PRIVATE ${OPENMESH_LIBRARIES})
target_link_libraries(march_iso PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(march_iso PRIVATE Eigen3::Eigen)
target_link_libraries(march_iso PRIVATE nlohmann_json::nlohmann_json)

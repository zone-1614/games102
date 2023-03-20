#include <fstream>
#include <tuple>

#include <rbf/util.h>
#include <rbf/MarchingCube.h>

#include <nlohmann/json.hpp>

using namespace zone;

std::tuple<func, Point, Point, std::string> read_json(const std::string& json_file_name);

int main(int argc, char** argv) {
    std::string mc_filename;
    if (argc == 1) {
        mc_filename = "mc_Arma.obj";
    } else {
        mc_filename = std::string(argv[1]);
    }
    auto [f, lfu, rbd, big_mesh_filename] = read_json("func.json");
    Mesh big_mesh;
    OpenMesh::IO::read_mesh(big_mesh, std::string(model_path) + big_mesh_filename);
    auto kdt = kdtree(big_mesh);
    Mesh mesh = marching_cube(f, lfu, rbd, kdt);
    std::string mc_path = std::string(model_path) + mc_filename;
    OpenMesh::IO::write_mesh(mesh, mc_path);
}

std::tuple<func, Point, Point, std::string> read_json(const std::string& json_file_name) {
    spdlog::info("read func and bounding box from file: {}", json_file_name);
    std::ifstream ifs(std::string(log_path) + json_file_name);
    nlohmann::json j;
    ifs >> j;

    func f;
    f.set_kernel(j["k"].get<int>());
    f.coefs = j["coefs"].get<std::vector<double>>();
    auto ps_array = j["ps"].get<std::vector<std::array<double, 3>>>();
    for (auto& arr : ps_array) {
        f.ps.push_back({ arr[0], arr[1], arr[2] });
    }
    spdlog::info("read func success");

    auto lfu_array = j["lfu"].get<std::array<double, 3>>();
    auto rbd_array = j["rbd"].get<std::array<double, 3>>();
    Point lfu, rbd;
    lfu[0] = lfu_array[0];
    lfu[1] = lfu_array[1];
    lfu[2] = lfu_array[2];
    rbd[0] = rbd_array[0];
    rbd[1] = rbd_array[1];
    rbd[2] = rbd_array[2];
    spdlog::info("read bounding box success");

    std::string big_mesh_filename = j["big_mesh_filename"].get<std::string>();
    return { f, lfu, rbd, big_mesh_filename };
}
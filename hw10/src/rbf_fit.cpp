#include <iostream>
#include <fstream>
#include <sstream>

#include <rbf/RBF.h>
#include <rbf/kdtree.h>

#include <nlohmann/json.hpp>

using namespace zone;

void save_func_and_boudingbox(const func& f, const RBF& rbf);

int main(int argc, char** argv) {
    std::string small_mesh, big_mesh, output_mesh;
    if (argc == 1) {
        small_mesh = "Arma_04.obj";
        big_mesh = "Arma.obj";
        output_mesh = "output_Arma.obj";
    } else {
        small_mesh = argv[1];
        big_mesh = argv[2];
        output_mesh = argv[3];
    }
    RBF rbf(small_mesh, big_mesh, output_mesh);
    func f = rbf.fit();
    save_func_and_boudingbox(f, rbf);
}

void save_func_and_boudingbox(const func& f, const RBF& rbf) {
    nlohmann::json j;
    j["k"] = f.k;
    j["coefs"] = f.coefs;
    j["ps"] = f.ps;
    
    auto [lfu, rbd] = rbf.get_boundingbox();
    j["lfu"] = lfu;
    j["rbd"] = rbd;
    j["big_mesh_filename"] = rbf.big_mesh_filename();

    std::ofstream ofs(std::string(log_path) + "func.json");
    ofs << std::setw(4) << j << std::endl;
    spdlog::info("save the interpolant as json");
}
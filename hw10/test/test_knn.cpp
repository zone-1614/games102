#include <rbf/kdtree.h>
using namespace zone;
int main() {
    Mesh mesh;
    OpenMesh::IO::read_mesh(mesh, model_path + std::string("Arma_04.obj"));
    kdtree kdt(mesh);
    for (auto v : mesh.vertices()) {
        auto p = mesh.point(v);
        spdlog::info("v---- {}, {}, {}", p[0], p[1], p[2]);
        auto knn = kdt.k_nearest(4, v);
        spdlog::info("knn size {}", knn.size());
        for (auto node : knn) {
            spdlog::info("knn----  {}, {}, {}", node->p[0], node->p[1], node->p[2]);
        }
    }
    return 0;
}
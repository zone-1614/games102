#include <rbf/kdtree.h>
using namespace zone;
int main() {
    Mesh mesh;
    OpenMesh::IO::read_mesh(mesh, model_path + std::string("Arma_04.obj"));
    kdtree kdt(mesh);
    return 0;
}
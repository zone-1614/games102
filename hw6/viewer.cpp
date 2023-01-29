#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <igl/opengl/glfw/Viewer.h>

typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;

Eigen::MatrixXd V;
Eigen::MatrixXi F;

int main(int argc, char* argv[]) {
    std::string file = "./model/Nefertiti_face.obj";
    if (argc == 2) {
        file = argv[1];
    }
    MyMesh mesh;

    if (!OpenMesh::IO::read_mesh(mesh, file)) return 1;

    igl::read_triangle_mesh(file, V, F);

    igl::opengl::glfw::Viewer viewer;
    
    viewer.data().set_mesh(V, F);
    viewer.launch();


}
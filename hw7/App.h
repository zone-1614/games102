#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Dense>

namespace zone {

enum class Weight { AVERAGE, COTANGENT };
enum class Boundary { CIRCLE, SQUARE };

using Mesh = OpenMesh::TriMesh_ArrayKernelT<>;
class App {
public:

    App(std::string i_filename, std::string o_filename);

    void launch();

private:
    void minimal_surface(Weight w);
    void parameterazation(Boundary b, Weight w);

    std::tuple<Eigen::SparseMatrix<double>, Eigen::MatrixXd> build_Laplacian(Weight w);
    Eigen::MatrixXd solve_Laplacian(const Eigen::SparseMatrix<double>& L, const Eigen::MatrixXd& B);
    void update_coord(const Eigen::MatrixXd& X);
    double clamp_cot(const OpenMesh::SmartHalfedgeHandle& hh);

    std::vector<Mesh::VertexHandle> get_boundary_in_order();
    void map_boundary_to_circle(std::vector<Mesh::VertexHandle>& boundary);
    void map_boundary_to_square(std::vector<Mesh::VertexHandle>& boundary);
private:
    std::string input_filename;
    std::string output_filename;

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    
    Mesh mesh;

    igl::opengl::glfw::Viewer viewer;
};

}
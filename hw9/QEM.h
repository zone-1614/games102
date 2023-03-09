#pragma once

#include <iostream>
#include <string>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "config.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Dense>
#include <spdlog/spdlog.h>

namespace zone {

using Mesh = OpenMesh::TriMesh_ArrayKernelT<>;
using Q_Matrix = Eigen::Matrix4f;
using abcd = std::tuple<float, float, float, float>;

class QEM {
public:
    QEM(std::string input_filename, std::string output_filename);

public:
    struct qem_pair {
        OpenMesh::SmartVertexHandle v0, v1;
        Mesh::Point p;
        float error;
        Mesh::HalfedgeHandle heh;
        Q_Matrix Q;

        // whether the vertex haven update?
        int v0_update = 0;
        int v1_update = 0;

        bool operator>(const qem_pair& rhs) const {
            return error > rhs.error;
        }
    };
    void usage();

    void qem_removal();

    void qem_make_Q();
    void qem_contract_edge();
    abcd get_face_abcd(const OpenMesh::SmartFaceHandle& f);
    qem_pair make_qem_pair(const OpenMesh::SmartHalfedgeHandle& seh);

private:
    std::string i_filename, o_filename;

    Mesh mesh;

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    igl::opengl::glfw::Viewer viewer;

    int t = 10; // delete points

    float costs = 0.0f;

    std::priority_queue<qem_pair, std::vector<qem_pair>, std::greater<qem_pair>> heap;
};

}
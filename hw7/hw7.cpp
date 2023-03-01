#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Sparse>
#include <vector>
#include <unordered_map>

using MyMesh = OpenMesh::TriMesh_ArrayKernelT<>;

Eigen::MatrixXd V;
Eigen::MatrixXi F;
std::string file = "./model/Nefertiti_face.obj";
std::string output_file = "./model/output.obj";
MyMesh mesh;

// 全局法极小曲面算法, 采用平均权
void minimal_surface_avg_weight() {
    const int nv = mesh.n_vertices();
    // 构建矩阵L, b
    std::vector<Eigen::Triplet<double>> L_tri;
    Eigen::MatrixXd B(nv, 3);
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            L_tri.push_back({ v.idx(), v.idx(), 1.0 });
            
            B(v.idx(), 0) = mesh.point(v)[0];
            B(v.idx(), 1) = mesh.point(v)[1];
            B(v.idx(), 2) = mesh.point(v)[2];
        } else {
            for (auto he : v.incoming_halfedges()) {
                if (he.is_valid()) {
                    L_tri.push_back({ v.idx(), he.from().idx(), -1.0 });
                }
            }
            int nn = v.outgoing_halfedges().to_vector().size(); // number of neighborhood
            L_tri.push_back({ v.idx(), v.idx(), (double)nn });

            B(v.idx(), 0) = 0.0;
            B(v.idx(), 1) = 0.0;
            B(v.idx(), 2) = 0.0;
        }
    }
    Eigen::SparseMatrix<double> L(nv, nv);
    L.setFromTriplets(L_tri.begin(), L_tri.end());

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver(L);
    Eigen::MatrixXd X = solver.solve(B);

    for (auto v : mesh.vertices()) {
        if (!v.is_boundary()) {
            mesh.set_point(v, { X(v.idx(), 0), X(v.idx(), 1), X(v.idx(), 2) });
        }
    }
}

// 采用cot 权的全局极小曲面算法
void minimal_surface_cot_weight() {
    const int nv = mesh.n_vertices();
    // 构建矩阵L, b
    std::vector<Eigen::Triplet<double>> L_tri;
    Eigen::MatrixXd B(nv, 3);
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            L_tri.push_back({ v.idx(), v.idx(), 1.0 });
            
            B(v.idx(), 0) = mesh.point(v)[0];
            B(v.idx(), 1) = mesh.point(v)[1];
            B(v.idx(), 2) = mesh.point(v)[2];
        } else {
            double tot = 0.0; // 总的权重
            auto angle = [](OpenMesh::SmartHalfedgeHandle& he) -> double {
                auto p1 = mesh.point(he.from()), p2 = mesh.point(he.to());
                auto p3 = mesh.point(he.next().to());
                // 用余弦定理求余弦
                double a = OpenMesh::norm(p1 - p3), b = OpenMesh::norm(p2 - p3), c = OpenMesh::norm(p1 - p2);
                double cosine = (a*a + b*b - c*c) / (2 * a * b);
                return std::acos(cosine);
            };
            for (auto he : v.outgoing_halfedges()) {
                if (he.is_valid()) {
                    double a1 = angle(he), a2 = angle(he.opp());
                    double weight = 1.0 / std::tan(a1) + 1.0 / std::tan(a2);
                    std::cout << "weight: " << weight << std::endl;
                    tot += weight;
                    L_tri.push_back({ v.idx(), he.to().idx(), -weight });
                }
            }
            std::cout << "tot: " << tot << std::endl;
            L_tri.push_back({ v.idx(), v.idx(), tot });

            B(v.idx(), 0) = 0.0;
            B(v.idx(), 1) = 0.0;
            B(v.idx(), 2) = 0.0;
        }
    }
    Eigen::SparseMatrix<double> L(nv, nv);
    L.setFromTriplets(L_tri.begin(), L_tri.end());

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver(L);
    Eigen::MatrixXd X = solver.solve(B);

    for (auto v : mesh.vertices()) {
        if (!v.is_boundary()) {
            mesh.set_point(v, { X(v.idx(), 0), X(v.idx(), 1), X(v.idx(), 2) });
        }
    }
}

// 均匀参数化到圆上, 使用平均权
void uniform_parameterazation(double r) {
    // 1.找出边界点
    std::vector<MyMesh::VertexHandle> boundary;
    MyMesh::VertexHandle prev, curr, start;
    // 1.1 找到第一个边界点
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            start = v;
            break;
        }
    }
    boundary.push_back(start);
    // 1.2 找出下一个边界点
    for (auto v : mesh.vv_range(start)) {
        if (v.is_boundary()) {
            prev = start;
            curr = v;
            break;
        }
    }
    // 1.3 循环找出所有边界点
    while (curr != start) {
        for (auto v : mesh.vv_range(curr)) {
            if (v.is_boundary() && v != prev) {
                boundary.push_back(curr);
                prev = curr;
                curr = v;
                break;
            }
        }
    }

    // 2. 把边界映射到单位圆上
    double d = 2.0 * M_PI / boundary.size();
    int i = 0;
    for (auto v : boundary) {
        mesh.set_point(v, { r * cos(i * d), r * sin(i * d), 0 });
        i++;
    }

    // 3. 解方程组
    minimal_surface_avg_weight();
}

// 参数化到正方形上, 使用cot权
void uniform_parameterazation_squared() {
    // 找出所有边界点
    std::vector<MyMesh::VertexHandle> boundary;
    MyMesh::VertexHandle prev, curr, start;
    // 找 start
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            start = v;
            break;
        }
    }
    boundary.push_back(start);
    // 找下一个边界
    for (auto v : mesh.vv_range(start)) {
        if (v.is_boundary()) {
            prev = start;
            curr = v;
            break;
        }
    }
    // 找所有边界
    while (curr != start) {
        for (auto v : mesh.vv_range(curr)) {
            if (v.is_boundary() && v != prev) {
                boundary.push_back(curr);
                prev = curr;
                curr = v;
                break;
            }
        }
    }

    // 2. 把边界映射到正方形上 (需要保证每个角都刚好有一个点)
    int n = boundary.size();
    int d = n / 4 + 1;
    double dw = 1.0 / d;
    double w = 0.0;
    for (int i = 0; i < d; i++) {
        mesh.set_point(boundary[i], { w, 0.0, 0.0 });
        std::cout << w << std::endl;
        w += dw;
    }
    mesh.set_point(boundary[d], { 1.0, 0.0, 0.0 });
    w = dw;
    for (int i = d + 1; i < 2 * d; i++) {
        mesh.set_point(boundary[i], { 1.0, w, 0.0 });
        std::cout << w << std::endl;
        w += dw;
    }
    mesh.set_point(boundary[2 * d], { 1.0, 1.0, 0.0 });
    w = dw;
    for (int i = 2 * d + 1; i < 3 * d; i++) {
        mesh.set_point(boundary[i], { 1.0 - w, 1.0, 0.0 });
        std::cout << 1.0 - w << std::endl;
        w += dw;
    }
    mesh.set_point(boundary[3 * d], { 0.0, 1.0, 0.0 });
    w = dw;
    for (int i = 3 * d + 1; i < n; i++) {
        mesh.set_point(boundary[i], { 0.0, 1.0 - w, 0.0 });
        std::cout << 1.0 - w << std::endl;
        w += dw;
    }

    // 3. 解方程组
    minimal_surface_cot_weight();
    // minimal_surface_avg_weight();
}

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier) {
    if (key == '1') {
        // 重新加载
        igl::read_triangle_mesh(file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '2') {
        minimal_surface_avg_weight();
        // 重新写入文件, 重新读
        OpenMesh::IO::write_mesh(mesh, output_file);
        igl::read_triangle_mesh(output_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '3') {
        minimal_surface_cot_weight();
        // 重新写入文件, 重新读
        OpenMesh::IO::write_mesh(mesh, output_file);
        igl::read_triangle_mesh(output_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '4') {
        uniform_parameterazation(1.0);
        OpenMesh::IO::write_mesh(mesh, output_file);
        igl::read_triangle_mesh(output_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '5') {
        uniform_parameterazation_squared();
        OpenMesh::IO::write_mesh(mesh, output_file);
        igl::read_triangle_mesh(output_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        file = argv[1];
    }

    if (!OpenMesh::IO::read_mesh(mesh, file)) return 1;

    igl::read_triangle_mesh(file, V, F);
    igl::opengl::glfw::Viewer viewer;
    viewer.callback_key_down = &key_down;
    viewer.data().set_mesh(V, F);
    viewer.launch();

    return 0;
}
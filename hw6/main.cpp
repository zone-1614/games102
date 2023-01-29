#include <iostream>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <igl/opengl/glfw/Viewer.h>
#include <fmt/core.h>
#include <vector>
#include <unordered_set>

typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;

double alpha = 0.2; // 迭代参数
Eigen::MatrixXd V;
Eigen::MatrixXi F;
std::string file = "./model/Nefertiti_face.obj";
std::string temp_file = "./model/temp.obj";
MyMesh mesh;

// 极小曲面算法
void minimal_surface(MyMesh& mesh) {
    MyMesh old = mesh;
    std::unordered_set<MyMesh::VertexHandle> boundary;
    // 1. 找到边界点, 并固定
    // 对于半边数据结构, 如果半边对应的面不存在, 说明是边界(翻了一下源码, 确实是这样实现的)
    // OpenMesh可以直接用 is_boundary() 判断是不是边界
    for (auto halfedge : old.halfedges()) {
        if (halfedge.is_boundary()) {
            boundary.insert(halfedge.from());
            boundary.insert(halfedge.to());
        }
    }

    // 2. 对内部点, 求新的坐标
    for (auto vertex : old.vertices()) {
        if (boundary.find(vertex) != boundary.end()) {
            continue;
        }

        // 找 1-邻域
        OpenMesh::DefaultTraits::Point delta = old.point(vertex);
        int n = vertex.outgoing_halfedges().to_vector().size();
        for (auto he : vertex.outgoing_halfedges()) {
            auto nv = he.to();
            delta -= old.point(nv) * (1.0 / n);
        }
        mesh.set_point(vertex, old.point(vertex) - alpha * delta);
    }
}

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier) {
    if (key == '1') {
        // 迭代
        minimal_surface(mesh);
        // 重新写入文件, 重新读
        OpenMesh::IO::write_mesh(mesh, temp_file);
        igl::read_triangle_mesh(temp_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '2') {
        // 迭代 10 次
        for (int i = 0; i < 10; i++) {
            minimal_surface(mesh);
        }
        // 重新写入文件, 重新读
        OpenMesh::IO::write_mesh(mesh, temp_file);
        igl::read_triangle_mesh(temp_file, V, F);
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
    } else if (key == '3') {
        if (alpha < 0.85) {
            fmt::print("iteration parameter alpha: {:.1} -> {:.1}\n", alpha, alpha + 0.1);
            alpha += 0.1;
        } else {
            fmt::print("iteration parameter could not greater than 1\n");
        }
    } else if (key == '4') {
        if (alpha > 0.15) {
            fmt::print("iteration parameter alpha: {:.1} -> {:.1}\n", alpha, alpha - 0.1);
            alpha -= 0.1;
        } else {
            fmt::print("iteration parameter could not less than 1\n");
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        file = argv[1];
    }

    if (!OpenMesh::IO::read_mesh(mesh, file)) return 1;

    std::cout << R"(
        1 迭代 1 次
        2 迭代 10 次
        3 迭代参数 增加0.1
        4 迭代参数 减少0.1
    )";

    igl::read_triangle_mesh(file, V, F);
    igl::opengl::glfw::Viewer viewer;
    viewer.callback_key_down = &key_down;
    viewer.data().set_mesh(V, F);
    viewer.launch();


}
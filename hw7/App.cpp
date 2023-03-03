#include "App.h"
#include "config.h"

#include <spdlog/spdlog.h>

namespace zone {

App::App(std::string i_filename, std::string o_filename)
    : input_filename(model_path + i_filename), output_filename(model_path + o_filename) 
{
    spdlog::info("Input filename: {}", input_filename);
    spdlog::info("Output filename: {}", output_filename);

    if (!OpenMesh::IO::read_mesh(mesh, input_filename)) {
        spdlog::throw_spdlog_ex("OpenMesh open file failed");
    }

    igl::read_triangle_mesh(input_filename, V, F);
    
    spdlog::info("Operations: ");
    spdlog::info("1: reload the origin model");
    spdlog::info("2: global minimal surface algorithm with avg weight");
    spdlog::info("3: global minimal surface algorithm with cot weight");
    spdlog::info("4: parameterazation to a circle with avg weight");
    spdlog::info("5: parameterazation to a circle with cot weight");
    spdlog::info("6: parameterazation to a square with avg weight");
    spdlog::info("7: parameterazation to a square with cot weight");
    viewer.callback_key_down = [&](igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier) -> bool {
        if (key == '1') { // reload the origin model
            spdlog::info("reload the origin model");
            igl::read_triangle_mesh(input_filename, V, F);
        } else if (key == '2') { 
            spdlog::info("global minimal surface algorithm with avg weight");
            minimal_surface(Weight::AVERAGE);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else if (key == '3') { 
            spdlog::info("global minimal surface algorithm with cot weight");
            minimal_surface(Weight::COTANGENT);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else if (key == '4') { 
            spdlog::info("parameterazation to a circle with avg weight");
            parameterazation(Boundary::CIRCLE ,Weight::AVERAGE);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else if (key == '5') { 
            spdlog::info("parameterazation to a circle with cot weight");
            parameterazation(Boundary::CIRCLE ,Weight::COTANGENT);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else if (key == '6') {
            spdlog::info("parameterazation to a square with avg weight");
            parameterazation(Boundary::SQUARE ,Weight::AVERAGE);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else if (key == '7') {
            spdlog::info("parameterazation to a square with cot weight");
            parameterazation(Boundary::SQUARE ,Weight::COTANGENT);
            OpenMesh::IO::write_mesh(mesh, output_filename);
            igl::read_triangle_mesh(output_filename, V, F);
        } else {
            spdlog::info("Unexpected key");
            return false;
        }
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.core().align_camera_center(V, F);
        return false;
    };

    viewer.data().set_mesh(V, F);
}

void App::launch() {
    spdlog::info("Launch the application");
    viewer.launch();
}

void App::minimal_surface(Weight w) {
    auto [L, B] = build_Laplacian(w);
    auto X = solve_Laplacian(L, B);
    update_coord(X);
}

void App::parameterazation(Boundary b, Weight w) {
    auto boundary = get_boundary_in_order();

    switch (b) {
        case Boundary::CIRCLE: {
            map_boundary_to_circle(boundary);
            break;
        }
        case Boundary::SQUARE: {
            map_boundary_to_square(boundary);
            break;
        }
    }

    minimal_surface(w);
}

std::tuple<Eigen::SparseMatrix<double>, Eigen::MatrixXd> App::build_Laplacian(Weight w) {
    const int nv = mesh.n_vertices(); // number of vertices
    // construct the Laplacian matrix L and the vector b
    std::vector<Eigen::Triplet<double>> L_tri;
    Eigen::MatrixXd B(nv, 3);
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            L_tri.push_back({ v.idx(), v.idx(), 1.0 });
            auto p = mesh.point(v);
            B.row(v.idx()) = Eigen::Vector3d(p[0], p[1], p[2]);
        } else {
            switch (w) {
                case Weight::AVERAGE: {
                    for (auto nei : mesh.vv_range(v)) { // neighborhood
                        L_tri.push_back({ v.idx(), nei.idx(), -1.0 });
                    }
                    int nn = v.outgoing_halfedges().to_vector().size(); // number of neighborhoods
                    L_tri.push_back({ v.idx(), v.idx(), (double) nn });
                    B.row(v.idx()) = Eigen::Vector3d::Zero();
                    break;
                }
                case Weight::COTANGENT: {
                    double tot = 0.0; // total weight
                    for (auto hh : v.outgoing_halfedges()) {
                        double w = clamp_cot(hh) + clamp_cot(hh.opp());
                        L_tri.push_back({ v.idx(), hh.to().idx(), -w });
                        tot += w;
                    }
                    L_tri.push_back({ v.idx(), v.idx(), tot });
                    B.row(v.idx()) = Eigen::Vector3d::Zero();
                    break;
                }
            }
        }
    }
    Eigen::SparseMatrix<double> L(nv, nv);
    L.setFromTriplets(L_tri.begin(), L_tri.end());

    return { L, B };
}

Eigen::MatrixXd App::solve_Laplacian(const Eigen::SparseMatrix<double>& L, const Eigen::MatrixXd& B) {
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver(L);
    return solver.solve(B);
}

void App::update_coord(const Eigen::MatrixXd& X) {
    for (auto v : mesh.vertices()) {
        if (!v.is_boundary()) {
            Mesh::Point new_point = { X(v.idx(), 0), X(v.idx(), 1), X(v.idx(), 2) };
            mesh.set_point(v, new_point);
        }
    }
}

double App::clamp_cot(const OpenMesh::SmartHalfedgeHandle& hh) {
    Mesh::Point p1 = mesh.point(hh.from()), p2 = mesh.point(hh.to());
    Mesh::Point p3 = mesh.point(hh.next().to());
    // cosine rule
    double a = OpenMesh::norm(p1 - p3), b = OpenMesh::norm(p2 - p3), c=  OpenMesh::norm(p1 - p2);
    double cosine = (a*a + b*b - c*c) / (2 * a * b);
    double cot = cosine / sqrt(1.0 - cosine * cosine);
    return std::clamp(cot, 0.5, 10.0);
}

std::vector<Mesh::VertexHandle> App::get_boundary_in_order() {
    // find first boundary point
    Mesh::VertexHandle start, prev, curr;
    std::vector<Mesh::VertexHandle> boundary;
    for (auto v : mesh.vertices()) {
        if (v.is_boundary()) {
            start = v;
            break;
        }
    }
    boundary.push_back(start);
    for (auto v : mesh.vv_range(start)) {
        if (v.is_boundary()) {
            prev = start;
            curr = v;
            break;
        }
    }
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
    return boundary;
}

void App::map_boundary_to_circle(std::vector<Mesh::VertexHandle>& boundary) {
    const int n = boundary.size();
    double step = 2.0 * M_PI / n;
    int i = 0;
    for (auto v : boundary) {
        mesh.set_point(v, { cos(i * step), sin(i * step), 0.0 });
        i++;
    }
}

void App::map_boundary_to_square(std::vector<Mesh::VertexHandle>& boundary) {
    const int n = boundary.size();
    const int d = n / 4 + 1;
    double w = 0.0, dw = 1.0 / d;

    for (int i = 0; i < d; i++, w += dw) {
        mesh.set_point(boundary[i], { 0.0 + w, 0.0, 0.0 });
    }
    mesh.set_point(boundary[d], { 1.0, 0.0, 0.0 });

    w = dw;
    for (int i = d + 1; i < 2 * d; i++, w += dw) {
        mesh.set_point(boundary[i], { 1.0, 0.0 + w, 0.0 });
    }
    mesh.set_point(boundary[2 * d], { 1.0, 1.0, 0.0 });
    
    w = dw;
    for (int i = 2 * d + 1; i < 3 * d; i++, w += dw) {
        mesh.set_point(boundary[i], { 1.0 - w, 1.0, 0.0 });
    }
    mesh.set_point(boundary[3 * d], { 0.0, 1.0, 0.0 });

    w = dw;
    for (int i = 3 * d + 1; i < n; i++, w += dw) {
        mesh.set_point(boundary[i], { 0.0, 1.0 - w, 0.0 });
    }
}

}
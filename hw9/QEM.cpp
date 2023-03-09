#include "QEM.h"

namespace zone {

QEM::QEM(std::string input_filename, std::string output_filename) {
    i_filename = model_path + input_filename;
    o_filename = model_path + output_filename;

    mesh.request_face_normals();
    OpenMesh::IO::read_mesh(mesh, i_filename);
    mesh.update_face_normals(); // if the model has no normal, it will update the normal
    igl::read_triangle_mesh(i_filename, V, F);

    usage();
    viewer.data().set_mesh(V, F);
    viewer.callback_key_down = [&](igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier) -> bool {
        if (key == '1') {
            qem_removal();
            OpenMesh::IO::write_mesh(mesh, o_filename);
            mesh.clear(); // clear will clean the custom property
            OpenMesh::IO::read_mesh(mesh, o_filename);
            spdlog::info("number of vertices = {}", mesh.n_vertices());
            mesh.update_face_normals();
            igl::read_triangle_mesh(o_filename, V, F);
            viewer.data().clear();
            viewer.data().set_mesh(V, F);
            viewer.data().show_lines = true;
            viewer.core().align_camera_center(V, F);
            return true;
        } else if (key == '2') {
            t += 1;
        } else if (key == '3') {
            t -= 1;
        } else if (key == '4') {
            t += 10;
        } else if (key == '5') {
            t -= 10;
        } else if (key == '6') {
            t += 100;
        } else if (key == '7') {
            t -= 100;
        }
        spdlog::info("t = {}", t);
        return false;
    };
    usage();
    viewer.launch();
}

void QEM::usage() {
    spdlog::info("Usage");
    spdlog::info("1: delete t vertex");
    spdlog::info("2: t += 1");
    spdlog::info("3: t -= 1");
    spdlog::info("4: t += 10");
    spdlog::info("5: t -= 10");
    spdlog::info("6: t += 100");
    spdlog::info("7: t -= 100");
}

void QEM::qem_removal() {
    // define the openmesh property
    OpenMesh::VProp<Q_Matrix>(mesh, "Q");
    OpenMesh::VProp<int>(0, mesh, "flag");

    // 1. make Q matrix
    qem_make_Q();
    
    // 2. select all valid pair
    // 3. compute the optimal vertex
    // 4. push to the heap
    heap = {};
    for (auto e : mesh.edges()) {
        // the function make_qem_pair will compute the optimal new vertex
        heap.push(make_qem_pair(e.halfedge(0)));
    }

    // 5. edge contraction
    qem_contract_edge();
}

void QEM::qem_make_Q() {
    // for each vertex v, compute Q
    auto Q = OpenMesh::VProp<Q_Matrix>(mesh, "Q");
    for (auto v : mesh.vertices()) {
        Q_Matrix Qv, K;
        Qv.setZero();
        for (auto f : v.faces()) {
            auto [a, b, c, d] = get_face_abcd(f);
            K << a * a, a * b, a * c, a * d,
                 a * b, b * b, b * c, b * d,
                 a * c, b * c, c * c, c * d,
                 a * d, b * d, c * d, d * d;
            Qv += K;
        }
        Q[v] = Qv;
    }
}

void QEM::qem_contract_edge() {
    mesh.request_vertex_status();
    mesh.request_edge_status();
    mesh.request_face_status();
    auto Q = OpenMesh::VProp<Q_Matrix>(mesh, "Q");
    auto flag = OpenMesh::VProp<int>(0, mesh, "flag");
    int d = 0; // deleted so far
    while (d < t) {
        auto t = heap.top();
        heap.pop();
        if (mesh.status(t.v0).deleted() || mesh.status(t.v1).deleted()) continue;
        if (t.v0_update != flag[t.v0] || t.v1_update != flag[t.v1]) continue;
        Mesh::VertexHandle vh;
        if (mesh.is_collapse_ok(t.heh)) {
            mesh.collapse(t.heh);
            vh = t.v0;
            flag[t.v0]++;
            flag[t.v1]++;
        } else continue;
        // spdlog::info("error = {}", t.error);
        mesh.set_point(vh, t.p);
        Q[vh] = t.Q;

        // update the error of the related pairs
        for (auto e : mesh.voh_range(vh)) {
            qem_pair pair = make_qem_pair(e);
            pair.v0_update = flag[pair.v0];
            pair.v1_update = flag[pair.v1];
            heap.push(pair);
        }
        d++;
    }
    mesh.garbage_collection();
    mesh.update_face_normals();
}

abcd QEM::get_face_abcd(const OpenMesh::SmartFaceHandle& f) {
    // face equation: ax + by + cz + d = 0
    auto normal = mesh.normal(f);
    auto n = normal.normalize();
    float a = n[0], b = n[1], c = n[2];
    float d = 0.0f;
    auto p = mesh.point(f.halfedge().to());
    d = -(a * p[0] + b * p[1] + c * p[2]);
    return { a, b, c, d };
}

QEM::qem_pair QEM::make_qem_pair(const OpenMesh::SmartHalfedgeHandle& heh) {
    qem_pair pair;
    pair.heh = heh;
    auto v0 = heh.to(), v1 = heh.from();
    pair.v0 = v0;
    pair.v1 = v1;
    auto Q = OpenMesh::VProp<Q_Matrix>(mesh, "Q");
    pair.Q = Q[v0] + Q[v1];
    auto Qt = pair.Q;// temp Q
    Qt(3, 0) = 0.0f;
    Qt(3, 1) = 0.0f;
    Qt(3, 2) = 0.0f;
    Qt(3, 3) = 1.0f;
    Eigen::FullPivLU<Q_Matrix> lu(Qt);
    auto get_error = [&](const Mesh::Point& p) -> float {
        // float x = p[0], y = p[1], z = p[2];
        // return pair.Q(0, 0) * x * x + 2.0 * pair.Q(0, 1) * x * y + 2.0 * pair.Q(0, 2) * x * z + 2.0 * pair.Q(0, 3) * x + pair.Q(1, 1) * y * y + 2.0 * pair.Q(1, 2) * y * z + 2 * pair.Q(1, 3) * y + pair.Q(2, 2) * z * z + 2.0 * pair.Q(2, 3) * z + pair.Q(3, 3);
        Eigen::Vector4f vv;
        vv << p[0], p[1], p[2], 1.0f;
        return vv.transpose() * pair.Q * vv;
    };
    // if the new Q matrix is invertiable, compute the optimal vertex
    if (lu.isInvertible()) {
        Eigen::Vector4f b;
        b << 0.0f, 0.0f, 0.0f, 1.0f;
        Eigen::Vector4f optimal_vertex = lu.solve(b); // homogenous optimal vertex
        Mesh::Point optimal_p;
        optimal_p[0] = optimal_vertex(0);
        optimal_p[1] = optimal_vertex(1);
        optimal_p[2] = optimal_vertex(2);
        pair.p = optimal_p;
        pair.error = get_error(optimal_p);
    } else {
        auto p0 = mesh.point(v0), p1 = mesh.point(v1), pp = (p0 + p1) / 2.0;
        pair.p = pp;
        pair.error = get_error(pp);
    }
    return pair;
}

}
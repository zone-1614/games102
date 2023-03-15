#include <rbf/RBF.h>
#include <Eigen/Dense>

namespace zone {

RBF::RBF(std::string input_filename, std::string output_filename) {
    i_filename = model_path + input_filename;
    o_filename = model_path + output_filename;

    mesh.request_vertex_normals();
    OpenMesh::IO::read_mesh(mesh, i_filename);
    kdt = kdtree(mesh);
    compute_all_normal();
    normal_orientation();
    gen_signed_field();
    // save s_mesh for debug
    OpenMesh::IO::write_mesh(s_mesh, o_filename);
}

void RBF::compute_all_normal() {
    for (auto v : mesh.vertices()) {
        auto knn = kdt.k_nearest(k, v);
        Normal normal = compute_normal(knn, v);
        mesh.set_normal(v, normal);
    }
}

Normal RBF::compute_normal(std::vector<kdnode_ptr>& knn, vh& v) {
    // construct the matrix M
    Point p = mesh.point(v);
    for (auto kp : knn) {
        p += kp->p;
    }
    p /= (knn.size() + 1);
    float xa = p[0], ya = p[1], za = p[2]; // x y z average
    Eigen::Matrix3f M;
    M.setZero();
    for (auto kp : knn) {
        auto xi = kp->p[0], yi = kp->p[1], zi = kp->p[2];
        Eigen::Vector3f vi;
        vi << xi - xa, yi - ya, zi - za;
        M += vi * vi.transpose();
    }

    // find the eigen vector of the minimum eigen value
    Eigen::EigenSolver<Eigen::Matrix3f> solver(M);
    auto eigen_vectors = solver.eigenvectors().real();
    auto eigen_values = solver.eigenvalues().real();
    auto min_ev = eigen_vectors.col(0);
    if (eigen_values(0, 0) > eigen_values(1, 0))
        min_ev = eigen_vectors.col(1);
    if (eigen_values(1, 0) > eigen_vectors(2, 0))
        min_ev = eigen_vectors.col(2);

    Normal normal;
    normal[0] = min_ev(0);
    normal[1] = min_ev(1);
    normal[2] = min_ev(2);
    return normal.normalize();
}

void RBF::normal_orientation() {
    // 1. construct the mst
    std::vector<vh> vhs;
    std::vector<std::vector<int>> topo; // topology of mst

    // index of a vertex handle in vhs
    auto index_of = [&](const vh& v) -> int {
        return std::find(vhs.begin(), vhs.end(), v) - vhs.begin();
    };
    for (auto v : mesh.vertices()) {
        vhs.push_back(v);
    }
    topo.resize(vhs.size());
    
    auto ist = OpenMesh::VProp<bool>(false, mesh, "ist"); // in spaning tree ?
    std::queue<vh> q;
    q.push(vhs.front());
    ist[vhs.front()] = true;
    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        auto knn = kdt.k_nearest(k, curr);
        std::vector<int> edges;
        for (auto nh : knn) {
            if (ist[nh->v]) continue;
            edges.push_back(index_of(nh->v));
            q.push(nh->v);
            ist[nh->v] = true;
        }
        topo[index_of(curr)] = edges;
    }

    // 2. orient the normal 
    std::queue<vh> qq;
    qq.push(vhs.front());
    while (!qq.empty()) {
        auto t = qq.front();
        auto n = mesh.normal(t);
        qq.pop();
        int idx = index_of(t);
        // for each child, orient the normal and push into the queue
        for (auto child : topo[idx]) {
            auto v = vhs[child];
            auto n2 = mesh.normal(v);
            if (n2.dot(n) < 0) {
                mesh.set_normal(v, -n2);
            }
            qq.push(v);
        }
    }
}

std::vector<std::pair<Point, float>> RBF::gen_signed_field() {
    std::vector<std::pair<Point, float>> data;
    for (auto v : mesh.vertices()) {
        data.push_back({ mesh.point(v), 0.0f });
    }
    s_mesh = mesh;
    for (auto v : mesh.vertices()) {
        auto p = mesh.point(v);
        auto n = mesh.normal(v);
        auto op1 = p + epsilon * n; // off-surface point
        auto op2 = p - epsilon * n;
        data.push_back({ op1, 1.0f });
        data.push_back({ op2, -1.0f });
        s_mesh.add_vertex(op1);
        s_mesh.add_vertex(op2);
    }
    return data;
}

float RBF::rbf_kernel(float x) {
    return std::expf(-c * x * x);
}

func RBF::fit() {
    int N = sf.size(), n = N + 4;
    // construct b
    Eigen::VectorXf b(n);
    b.setZero();
    for (int i = 0; i < N; i++) {
        b(i) = sf[i].second;
    }

    // contruct B
    Eigen::MatrixXf B(n, n);
    B.setZero();
    for (int i = 0; i < N; i++) {
        auto pi = sf[i].first;
        for (int j = 0; j < N; j++) {
            auto pj = sf[j].first;
            auto norm = (pi - pj).norm();
            auto phi_ij = rbf_kernel(norm);
            B(i, j) = phi_ij;
        }
        B(i, N) = pi[0];
        B(i, N + 1) = pi[1];
        B(i, N + 2) = pi[2];
        B(i, N + 3) = 1.0;
        
        B(N, i) = pi[0];
        B(N + 1, i) = pi[1];
        B(N + 2, i) = pi[2];
        B(N + 3, i) = 1.0;
    }

    // sovle
    Eigen::FullPivLU<Eigen::MatrixXf> solver(B);
    Eigen::VectorXf x = solver.solve(b);

    // monomial
    std::array<float, 4> arr;
    arr[0] = x(N);
    arr[1] = x(N + 1);
    arr[2] = x(N + 2);
    arr[3] = x(N + 3);

    // coefs
    std::vector<float> coefs;
    for (int i = 0; i < N; i++) {
        coefs.push_back(x(i));
    }

    func f;
    f.arr = arr;
    f.coefs = coefs;
    f.c = c;
    for (int i = 0; i < N; i++) {
        f.ps.push_back(sf[i].first);
    }
    return f;
}

}
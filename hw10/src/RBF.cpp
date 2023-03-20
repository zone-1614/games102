#include <rbf/RBF.h>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <spdlog/sinks/basic_file_sink.h>

namespace zone {

RBF::RBF(std::string small_mesh_, std::string big_mesh_, std::string output_mesh_) {
    big_mesh_filename_ = big_mesh_;
    small_mesh.request_vertex_normals();
    big_mesh.request_vertex_normals();
    //output_mesh.request_vertex_normals();
    OpenMesh::IO::read_mesh(small_mesh, std::string(model_path) + small_mesh_);
    OpenMesh::IO::read_mesh(big_mesh, std::string(model_path) + big_mesh_);
    //OpenMesh::IO::read_mesh(output_mesh, std::string(model_path) + output_mesh_);
    spdlog::info("OpenMesh read mesh success");

    kernel = zone::kernel1;

    small_kdt = kdtree(small_mesh);
    big_kdt = kdtree(big_mesh);

    spdlog::info("build kdtree success");
    compute_all_normal();
    spdlog::info("compute all normal success");
    normal_orientation();
    spdlog::info("normal orientation success");
    sf = gen_signed_field();
    spdlog::info("generate signed field success");

    OpenMesh::IO::write_mesh(output_mesh, std::string(model_path) + output_mesh_);
}

void RBF::compute_all_normal() {
    for (auto v : small_mesh.vertices()) {
        auto point = small_mesh.point(v);
        auto knn = big_kdt.k_nearest(k, point);
        auto normal = compute_normal(knn);
        small_mesh.set_normal(v, normal);
    }
}

Normal RBF::compute_normal(std::vector<kdnode_ptr>& knn) {
    // construct the matrix M
    std::vector<Point> ps;
    for (auto kp : knn) {
        ps.push_back(kp->p);
    }

    Point mean_p;
    // set zero 
    mean_p[0] = 0.0; 
    mean_p[1] = 0.0; 
    mean_p[2] = 0.0; 
    for (auto p : ps) {
        mean_p += p;
    }
    mean_p /= ps.size();
    double xa = mean_p[0], ya = mean_p[1], za = mean_p[2]; // x y z average
    Eigen::Matrix3d M;
    M.setZero();
    for (auto p : ps) {
        double xi = p[0], yi = p[1], zi = p[2];
        Eigen::Vector3d vi;
        vi << xi - xa, yi - ya, zi - za;
        M += vi * vi.transpose();
    }
    M /= ps.size();

    // find the eigen vector of the minimum eigen value
    Eigen::EigenSolver<Eigen::Matrix3d> solver(M);
    Eigen::Matrix3d eigen_vectors = solver.eigenvectors().real();
    Eigen::Vector3d eigen_values = solver.eigenvalues().real();
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

    auto ist = OpenMesh::VProp<bool>(false, small_mesh, "ist"); // in spaning tree ?
    for (auto v : small_mesh.vertices()) {
        vhs.push_back(v);
    }
    topo.resize(vhs.size());
    
    std::queue<vh> q;
    q.push(vhs.front());
    ist[vhs.front()] = true;
    while (!q.empty()) {
        auto curr = q.front();
        q.pop();
        auto knn = small_kdt.k_nearest(k, small_mesh.point(curr));
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
        auto n = small_mesh.normal(t);
        qq.pop();
        int idx = index_of(t);
        // for each child, orient the normal and push into the queue
        for (auto child : topo[idx]) {
            auto v = vhs[child];
            auto n2 = small_mesh.normal(v);
            qq.push(v);
        }
    }
}

std::vector<std::pair<Point, double>> RBF::gen_signed_field() {
    std::vector<std::pair<Point, double>> data;
    output_mesh = small_mesh;
    for (auto v : small_mesh.vertices()) {
        auto p = small_mesh.point(v);
        auto n = small_mesh.normal(v);

        data.push_back({ p, 0.0 });
        
        auto op1 = p + epsilon * n; // off-surface point
        data.push_back({ op1, 1.0 });
        output_mesh.add_vertex(op1);

        auto op2 = p - epsilon * n; // off-surface point
        data.push_back({ op2, -1.0 });
        output_mesh.add_vertex(op2);
    }
    return data;
}

// fitting function for dense matrix
func RBF::fit() {
    const size_t N = sf.size();
    Eigen::VectorXd b(N);
    Eigen::MatrixXd A(N, N);
    A.setZero();
    for (int i = 0; i < N; i++) {
        auto pi = sf[i].first;
        for (int j = 0; j < N; j++) {
            auto pj = sf[j].first;
            A(i, j) = kernel((pi - pj).norm());
        }
        b(i) = sf[i].second;
    }

    spdlog::info("solving the matrix. wait about three minutes");
    // solve
    Eigen::VectorXd x = A.householderQr().solve(b);
    spdlog::info("solve the matrix success");

    func f;
    f.set_kernel(1);
    for (int i = 0; i < N; i++) {
        f.coefs.push_back(x(i));
    }
    for (int i = 0; i < N; i++) {
        f.ps.push_back(sf[i].first);
    }

    return f;
}

// fitting function for sparse matrix
func RBF::fit_sparse() {
    kernel = zone::kernel3;
    const size_t N = sf.size();
    Eigen::VectorXd b(N);
    std::vector<Eigen::Triplet<double>> tri;
    Eigen::SparseMatrix<double> A(N, N);
    for (int i = 0; i < N; i++) {
        auto pi = sf[i].first;
        for (int j = 0; j < N; j++) {
            auto pj = sf[j].first;
            auto len = (pi - pj).norm();
            if (len < 0.2) {
                tri.push_back({ i, j, kernel(len) });
            }
        }
        b(i) = sf[i].second;
    }
    A.setFromTriplets(tri.begin(), tri.end());

    // solve
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver(A);
    Eigen::VectorXd x = solver.solve(b);
    
    func f;
    f.kernel = this->kernel;
    for (int i = 0; i < N; i++) {
        f.coefs.push_back(x(i));
    }
    for (int i = 0; i < N; i++) {
        f.ps.push_back(sf[i].first);
    }
    return f;
}

std::pair<Point, Point> RBF::get_boundingbox() const {
    double minx = DBL_MAX, miny = DBL_MAX, minz = DBL_MAX;
    double maxx = -DBL_MAX, maxy = -DBL_MAX, maxz = -DBL_MAX;
    for (auto v : output_mesh.vertices()) {
        auto p = output_mesh.point(v);
        minx = std::min((double)p[0], minx);
        miny = std::min((double)p[1], miny);
        minz = std::min((double)p[2], minz);
        maxx = std::max((double)p[0], maxx);
        maxy = std::max((double)p[1], maxy);
        maxz = std::max((double)p[2], maxz);
    }

    double dx = (maxx - minx) * 0.2, dy = (maxy - miny) * 0.2, dz = (maxz - minz) * 0.2;

    return {
        { maxx + dx, miny - dy, maxz + dz },
        { minx - dx, maxy + dy, minz - dz }
    };
}

std::string RBF::big_mesh_filename() const {
    return big_mesh_filename_;
}

}
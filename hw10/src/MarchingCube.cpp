#include <rbf/MarchingCube.h>
#include <rbf/octree.h>
#include <rbf/util.h>

namespace zone {

struct point_hasher {
    std::size_t operator()(const Point& p) const {
        return (std::hash<float>()(p[0])) ^ (std::hash<float>()(p[1])) ^ (std::hash<float>()(p[2]));
    }
};

Mesh marching_cube(const func& f, const Point& lfu, const Point& rbd, const kdtree& kdt) {
    // build octree to split the space
    spdlog::info("build octree");
    octree oct(f, lfu, rbd);
    spdlog::info("build octree success");
    spdlog::info("traverse octree");
    spdlog::info("octree leaf size {}", oct.n_leaf);
    Mesh mesh;
    double iso = 0.0;
    int n_march_cube = 0;
    double max_dis = oct.get_threshold() * 10.0;
    oct.traversal([&](onode_ptr op) {
        if (!op->is_leaf) return ;
        if (kdt.nearest_distance(op->middle()) > max_dis) return;
        int mc_index = 0;
        auto ps = op->gen_all_points();
        if (f(ps[0]) > iso) mc_index |= (1 << 0);
        if (f(ps[1]) > iso) mc_index |= (1 << 1);
        if (f(ps[2]) > iso) mc_index |= (1 << 2);
        if (f(ps[3]) > iso) mc_index |= (1 << 3);
        if (f(ps[4]) > iso) mc_index |= (1 << 4);
        if (f(ps[5]) > iso) mc_index |= (1 << 5);
        if (f(ps[6]) > iso) mc_index |= (1 << 6);
        if (f(ps[7]) > iso) mc_index |= (1 << 7);

        const int* face = a2fConnectionTable[mc_index];
        int i = 0;
        std::unordered_map<Point, vh, point_hasher> map;
        while (i < 16) {
            if (face[i] == -1) break;
            auto ps = op->gen_all_points();
            Point p1, p2, p3;
            const int* e1 = a2eConnection[face[i]], *e2 = a2eConnection[face[i + 1]], *e3 = a2eConnection[face[i + 2]];
            p1 = interpolate(f, ps[e1[0]], ps[e1[1]]);
            p2 = interpolate(f, ps[e2[0]], ps[e2[1]]);
            p3 = interpolate(f, ps[e3[0]], ps[e3[1]]);
            std::vector<vh> vs;
            if (map.find(p1) != map.end()) {
                vs.push_back(map[p1]);
            } else {
                auto vhandle = mesh.add_vertex(p1);
                vs.push_back(vhandle);
                map.emplace(p1, vhandle);
            }
            if (map.find(p2) != map.end()) {
                vs.push_back(map[p2]);
            } else {
                auto vhandle = mesh.add_vertex(p2);
                vs.push_back(vhandle);
                map.emplace(p2, vhandle);
            }
            if (map.find(p3) != map.end()) {
                vs.push_back(map[p3]);
            } else {
                auto vhandle = mesh.add_vertex(p3);
                vs.push_back(vhandle);
                map.emplace(p3, vhandle);
            }
            mesh.add_face(vs);
            i += 3;
        }

        n_march_cube++;
        if (n_march_cube % 100 == 0) {
            spdlog::info("march {} cubes", n_march_cube);
        }
    });
    spdlog::info("mesh n of vertices: {}", mesh.n_vertices());
    spdlog::info("mesh n of faces: {}", mesh.n_faces());
    return mesh;
}

Point interpolate(const func& f, const Point& p1, const Point& p2) {
    double fp1 = f(p1), fp2 = f(p2);
    double d = fp2 - fp1;
    double t = fp1 / d;
    return p1 - t * (p2 - p1);
    //return (p1 + p2) / 2.0;
}

}
#pragma once

#include <rbf/util.h>
#include <rbf/kdtree.h>

#include <string>
#include <array>

namespace zone {

struct func {
    std::vector<double> coefs;
    std::vector<Point> ps;
    std::function<double(double)> kernel = zone::kernel1;
    int k = 1;
    void set_kernel(int k_) {
        k = k_;
        if (k == 1) {
            kernel = zone::kernel1;
        } else if (k == 2) {
            kernel = zone::kernel2;
        } else if (k == 5) {
            kernel = zone::kernel5;
        }
    }
    double operator()(const Point& p) const {
        double rbf = 0.0;
        for (int i = 0; i < ps.size(); i++) {
            rbf += kernel((p - ps[i]).norm()) * coefs[i];
        }
        return rbf;
    }
};

class RBF {
public:
    RBF(std::string small_mesh_, std::string big_mesh_, std::string output_mesh_);
    func fit();
    func fit_sparse();
    std::pair<Point, Point> RBF::get_boundingbox() const;
    std::string big_mesh_filename() const;

private:
    void compute_all_normal();
    Normal compute_normal(std::vector<kdnode_ptr>& knn);
    void normal_orientation();
    std::vector<std::pair<Point, double>> gen_signed_field();
    void set_kernel(int k_);
private:
    std::string big_mesh_filename_;
    std::function<double(double)> kernel;
    int ker = 1;

    Mesh small_mesh, big_mesh, output_mesh;
    kdtree small_kdt, big_kdt;
    size_t k = 8;
    double epsilon = 0.5;
    std::vector<std::pair<zone::Point, double>> sf; // signed field

};

}
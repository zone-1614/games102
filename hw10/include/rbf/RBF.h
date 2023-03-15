#pragma once

#include <rbf/util.h>
#include <rbf/kdtree.h>

#include <string>
#include <array>

namespace zone {

struct func {
    std::vector<float> coefs;
    std::array<float, 4> arr;
    std::vector<Point> ps;
    float c;
    float kernel(float x) {
        return std::expf(-c * x * x);
    }
    float operator()(const Point& p) {
        float monomial = arr[0] * p[0] + arr[1] * p[1] + arr[2] * p[2] + arr[3];
        float rbf = 0.0f;
        for (int i = 0; i < ps.size(); i++) {
            rbf += kernel((p - ps[i]).norm()) * coefs[i];
        }
        return rbf + monomial;
    }
};

class RBF {
public:
    RBF(std::string input_filename, std::string output_filename);
    func fit();
    
private:
    void compute_all_normal();
    Normal compute_normal(std::vector<kdnode_ptr>& knn, vh& v);
    void normal_orientation();
    std::vector<std::pair<Point, float>> gen_signed_field();
    float rbf_kernel(float x); // gaussian
private:
    std::string i_filename, o_filename;

    Mesh mesh;
    Mesh s_mesh; // mesh used for signed field
    kdtree kdt;
    size_t k = 4;
    float epsilon = 0.02f;
    std::vector<std::pair<zone::Point, float>> sf; // signed field

    float c = 0.0f; // for gaussian
    
};

}
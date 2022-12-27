#pragma once
#include <vector>
#include <Eigen/Dense>

using std::vector;
using Eigen::Vector2f;

class zmath {
public:
    static vector<Vector2f> InterpolationPolygon(const vector<Vector2f> &in_pos, float lb, float rb, float step);
    
    static vector<Vector2f> InterpolationGauss(const vector<Vector2f> &in_pos, float sigma2, int m, float lb, float rb, float step);

    static vector<Vector2f> ApproximationPolygon(const vector<Vector2f> &in_pos, int m, float lb, float rb, float step);

    static vector<Vector2f> ApproximationNormalized(const vector<Vector2f> &in_pos, int m, float lambda, float lb, float rb, float step);
};
#include <rbf/util.h>

namespace zone {

std::string PointToString(const Point& p) {
    std::stringstream ss;
    ss << "(" << p[0] << ", " << p[1] << ", " << p[2] << ")";
    return ss.str();
}

double kernel1(double x) {
    return x;
}
double kernel2(double x) {
    return x * x * x;
}
double kernel3(double x) {
    return std::exp(-x * x * 50);
}
double kernel4(double x) {
    return x * x * std::log(x);
}

std::string EigenToString(const Eigen::MatrixXd& mat) {
    std::stringstream ss;
    ss << mat;
    return ss.str();
}

std::string EigenToString(const Eigen::VectorXd& mat) {
    std::stringstream ss;
    ss << mat;
    return ss.str();
}

}
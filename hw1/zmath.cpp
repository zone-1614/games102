#include "zmath.h"

using std::vector;
using Eigen::Vector2f;
using Eigen::MatrixXf;

constexpr float Sqr(float x) {
    return x * x;
}

MatrixXf LeastSquares(const vector<Vector2f> &in_pos, int m, float lambda = 0.0f) {
    const int n = in_pos.size();
    MatrixXf A(m + 1, m + 1);
    vector<float> pow_temp(n, 1.0f);
    for (int i = 0; i < 2 * m + 1; i++) {
        float sum = 0;
        for (int j = 0; j < n; j++) {
            sum += pow_temp[j];
            pow_temp[j] *= in_pos[j].x();
        }
        for (int j = 0; j <= i; j++) {
            if (j <= m && i - j <= m) {
                A(j, i - j) = sum;
            }
        }
    }

    MatrixXf Y(m + 1, 1);
    std::fill(pow_temp.begin(), pow_temp.end(), 1.0f);
    for (int i = 0; i <= m; i++) {
        Y(i, 0) = 0.0f;
        for (int j = 0; j < n; j++) {
            Y(i, 0) += in_pos[j].y() * pow_temp[j];
            pow_temp[j] *= in_pos[j].x();
        }
    }

    MatrixXf B = A.inverse() * Y;
    return B;
}

vector<Vector2f> zmath::InterpolationPolygon(const vector<Vector2f> &in_pos, float lb, float rb, float step) {
    vector<Vector2f> result;
    for (float x = lb; x <= rb; x += step) {
        float y = 0;
        for (int i = 0; i < in_pos.size(); i++) {
            float temp = in_pos[i].y();
            for (int j = 0; j < in_pos.size(); j++) {
                if (i != j) {
                    temp = temp * (x - in_pos[j].x()) / (in_pos[i].x() - in_pos[j].x());
                }
            }
            y += temp;
        }
        result.emplace_back(x, y);
    }
    return result;
}

vector<Vector2f>
zmath::InterpolationGauss(const vector<Vector2f> &in_pos, float sigma2, int m,
    float lb, float rb, float step) {
    const int n = in_pos.size();
    m = std::min(m, std::max(n - 1, 0));

    MatrixXf B_poly = LeastSquares(in_pos, m);
    vector<float> y_approx(n);
    for (int i = 0; i < n; i++) {
        float y = 0, x_temp = 1.0f;
        for (int j = 0; j <= m; j++) {
            y += B_poly(j, 0) * x_temp;
            x_temp *= in_pos[i].x();
        }
        y_approx[i] = y;
    }

    MatrixXf A(n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A(i, j) = std::exp(-Sqr(in_pos[i].x() - in_pos[j].x()) / (2 * sigma2));
        }
    }

    MatrixXf Y(n, 1);
    for (int i = 0; i < n; i++) {
        Y(i, 0) = in_pos[i].y() - y_approx[i];
    }

    MatrixXf B = A.inverse() * Y;
    vector<Vector2f> result;
    for (float x = lb; x <= rb; x += step) {
        float y = 0, x_temp = 1.0f;
        for (int i = 0; i <= m; i++) {
            y += B_poly(i, 0) * x_temp;
            x_temp *= x;
        }
        for (int i = 0; i < n; i++) {
            y += B(i, 0) * std::exp(-Sqr(x - in_pos[i].x()) / (2 * sigma2));
        }
        result.emplace_back(x, y);
    }
    return result;
}

vector<Vector2f>
zmath::ApproximationPolygon(const vector<Vector2f> &in_pos, int m, float lb, float rb, float step) {
    const int n = in_pos.size();
    m = std::min(m, std::max(n - 1, 0));
    MatrixXf B = LeastSquares(in_pos, m);
    vector<Vector2f> result;
    for (float x = lb; x <= rb; x += step) {
        float y = 0, x_temp = 1.0f;
        for (int i = 0; i <= m; i++) {
            y += B(i, 0) * x_temp;
            x_temp *= x;
        }
        result.emplace_back(x, y);
    }
    return result;
}

vector<Vector2f>
zmath::ApproximationNormalized(const vector<Vector2f> &in_pos, int m, float lambda,
    float lb, float rb, float step) {
    const int n = in_pos.size();
    m = std::min(m, std::max(n - 1, 0));
    MatrixXf B = LeastSquares(in_pos, m, lambda);
    vector<Vector2f> result;
    for (float x = lb; x <= rb; x += step) {
        float y = 0, x_temp = 1.0f;
        for (int i = 0; i <= m; i++) {
            y += B(i, 0) * x_temp;
            x_temp *= x;
        }
        result.emplace_back(x, y);
    }
    return result;
}
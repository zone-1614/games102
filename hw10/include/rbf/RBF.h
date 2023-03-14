#pragma once

#include <rbf/util.h>

#include <string>

namespace zone {

class RBF {
public:
    RBF(std::string input_filename, std::string output_filename);

private:
    // void compute_normal();

private:
    std::string i_filename, o_filename;

    Mesh mesh;
};

}
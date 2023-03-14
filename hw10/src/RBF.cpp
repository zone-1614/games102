#include <rbf/RBF.h>

namespace zone {

RBF::RBF(std::string input_filename, std::string output_filename) {
    i_filename = model_path + input_filename;
    o_filename = model_path + output_filename;

    OpenMesh::IO::read_mesh(mesh, i_filename);
}

}
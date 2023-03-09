#include "QEM.h"
using zone::QEM;

int main(int argc, char** argv) {
    if (argc != 3) {
        spdlog::info("Usage: hw9 [input-filename] [output-filename]");
        return 1;
    }
    std::string input_filename = argv[1];
    std::string output_filename = argv[2];
    
    QEM qem(input_filename, output_filename);
}

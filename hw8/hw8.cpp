#include <iostream>

#define JC_VORONOI_IMPLEMENTATION
#include "App.h"

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Usage: hw8 [number of point] [iter times]" << std::endl;
        return 1;
    }
    int n = 10, it = 20;
    std::string output_filename;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            n = atoi(argv[i+1]);
        } else if (strcmp(argv[i], "-i") == 0) {
            it = atoi(argv[i+1]); 
        } else if (strcmp(argv[i], "-o") == 0) {
            output_filename = std::string(argv[i+1]);
        }
    }

    zone::App app(n, it, output_filename);
    
    return 0;
}

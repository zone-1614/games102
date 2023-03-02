#include "App.h"
#include <cmath>
#include <spdlog/spdlog.h>
int main(int argc, char* argv[]) {
    std::string input_filename = "./model/Nefertiti_face.obj";
    if (argc == 2) {
        input_filename = argv[1];
    }
    zone::App app(input_filename, "./model/output.obj");
    app.launch();
    // spdlog::info("cos(pi) = {}, cos(pi / 2) = {}", cos(M_PI), cos(M_PI / 2));
    // spdlog::info("acos(1) = {}, cos(0) = {}", acos(1), acos(0));
    // spdlog::info("tan()", acos(1), acos(0));
}
#include "App.h"

int main(int argc, char* argv[]) {
    std::string input_filename = "Nefertiti_face.obj";
    if (argc == 2) {
        input_filename = argv[1];
    }
    zone::App app(input_filename, "output.obj");
    app.launch();
}
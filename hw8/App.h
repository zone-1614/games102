#pragma once

#include "config.h"
#include "jc_voronoi.h"

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>

#include <igl/opengl/glfw/Viewer.h>

namespace zone {

class App {
public:
    App(int num_of_point, int iter_times, std::string filename);

private:
    void init_png();
    void generate_random_points();
    void lloyd();
    jcv_point centroid(jcv_graphedge* ge);
    void jcv_diagram_to_png(std::string png_name);
    void jcv_diagram_to_obj();
    

    jcv_point remap(const jcv_point& p);
    void draw_point(int x, int y);
    void draw_big_point(int x, int y);
    void draw_line(int x0, int y0, int x1, int y1);

private:
    int n;
    int it;
    // output filename
    std::string output;

    // image 
    int width = 512;
    int height = 512;
    int nchannels = 3;
    unsigned char* image;
    unsigned char point_color[3] = { 178, 102, 255 };

    // jcv
    jcv_rect bounding_box = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
    jcv_diagram diagram{0};
    std::vector<jcv_point> points;
};

}
#include "App.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <igl/png/writePNG.h>

bool operator==(const jcv_point& lhs, const jcv_point& rhs) {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

namespace zone {

App::App(int num_of_point, int iter_times, std::string filename)
    : n(num_of_point), it(iter_times), output(project_path + filename) {
    init_png();
    generate_random_points();
    lloyd();
    jcv_diagram_to_png("init.png");
    init_png();
    for (int i = 0; i < it; i++) {
        points.clear();
        auto sites = jcv_diagram_get_sites(&diagram);
        for (int i = 0; i < diagram.numsites; i++) {
            auto s = &sites[i];
            points.push_back(centroid(s->edges));
        }
        lloyd();
    }
    jcv_diagram_to_png("result.png");
}

void App::init_png() {
    size_t imagesize = (size_t)(width * height * 3);
    image = (unsigned char*) malloc(imagesize);
    memset(image, 0, imagesize);
}

void App::generate_random_points() {
    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_real_distribution<float> dist(0.05f, 0.95f);
    for (int i = 0; i < n; i++) {
        jcv_point p;
        p.x = dist(e);
        p.y = dist(e);
        points.push_back(p);
    }
}

void App::lloyd() {
    if (diagram.internal) {
        jcv_diagram_free(&diagram);
    }
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(points.size(), points.data(), &bounding_box, 0, &diagram);
}

jcv_point App::centroid(jcv_graphedge* ge) {
    double total_det = 0;
    jcv_point p{0, 0};
    while (ge) {
        jcv_point p1 = ge->pos[0], p2 = ge->pos[1];
        double det = p1.x * p2.y - p2.x * p1.y;
        total_det += det;
        p.x += (p1.x + p2.x) * det;
        p.y += (p1.y + p2.y) * det;
        ge = ge->next;
    }
    p.x /= 3 * total_det;
    p.y /= 3 * total_det;
    return p;
}

void App::jcv_diagram_to_png(std::string png_name) {
    auto sites = jcv_diagram_get_sites(&diagram);
    for (int i = 0; i < diagram.numsites; i++) {
        auto s = &sites[i];
        auto p = remap(s->p);
        draw_big_point((int)p.x, (int)p.y);
    }

    const jcv_edge* edge = jcv_diagram_get_edges(&diagram);
    while (edge) {
        auto p0 = remap(edge->pos[0]), p1 = remap(edge->pos[1]);
        draw_line((int)p0.x, (int)p0.y, (int)p1.x, (int)p1.y);
        edge = jcv_diagram_get_next_edge(edge);
    }

    std::string png = project_path + png_name;
    stbi_write_png(png.c_str(), width, height, 3, image, 0);
}

jcv_point App::remap(const jcv_point& p) {
    jcv_point pp;
    pp.x = p.x * width;
    pp.y = p.y * height;
    return pp;
}

void App::draw_point(int x, int y) {
    if (x < 0 || y < 0 || x > (width - 1) || y > (height - 1))
        return;
    int index = y * width * nchannels + x * nchannels;
    for (int i = 0; i < nchannels; i++) {
        image[index + i] = point_color[i];
    }
}

void App::draw_big_point(int x, int y) {
    draw_point(x, y);
    draw_point(x + 1, y);
    draw_point(x - 1, y);
    draw_point(x, y + 1);
    draw_point(x + 1, y + 1);
    draw_point(x - 1, y + 1);
    draw_point(x, y - 1);
    draw_point(x + 1, y - 1);
    draw_point(x - 1, y - 1);
}

void App::draw_line(int x0, int y0, int x1, int y1) {
    int nowX = x0, nowY = y0;
    int delta = 0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int k1 = dx << 1;
    int k2 = dy << 1;
    int xStep = 1;
    int yStep = 1;

    if (dx < 0) {
        dx = -dx;
        k1 = -k1;
        xStep = -xStep;
    }
    if (dy < 0) {
        dy = -dy;
        k2 = -k2;
        yStep = -yStep;
    }
    draw_big_point(nowX, nowY);
    if (dx < dy) {
        while (nowY != y1) {
            nowY += yStep;
            delta += k1;
            if (delta >= dy) {
                nowX += xStep;
                dy += k2;
            }
            draw_big_point(nowX, nowY);
        }
    } else {
        while (nowX != x1) {
            nowX += xStep;
            delta += k2;
            if (delta >= dx) {
                nowY += yStep;
                dx += k1;
            }
            draw_big_point(nowX, nowY);
        }
    }
}

}
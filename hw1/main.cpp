#include <iostream>
#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

#include "zmath.h"

using std::vector;
using Eigen::Vector2f;

GLFWwindow* window = nullptr;
int width = 1200;
int height = 800;

// canvas 是独立在 window 里面的一个子窗口, 记下它的左上和右下才能画图
ImVec2 canvas_pos_ul = { 0.0f, 0.0f }; // upper left
ImVec2 canvas_pos_br = { 0.0f, 0.0f }; // bottom right

/**
 * @brief 画线, 同时画点
 * 
 * @param poss 所有的点
 * @param draw_list 
 * @param line_col 线的颜色  数据类型ImU32经常用来存颜色
 * @param point_col 点的颜色
 */
void PlotLineSegments(const vector<Vector2f> &poss, ImDrawList *draw_list, ImU32 line_col, ImU32 point_col) {
    for (size_t i = 1; i < poss.size(); i++) {
        draw_list->AddLine(
            { canvas_pos_ul.x + poss[i - 1].x(), canvas_pos_br.y - poss[i - 1].y() }, // point 1
            { canvas_pos_ul.x + poss[i].x(),     canvas_pos_br.y - poss[i].y() }, // point 2
            line_col, // color
            2.0f // thickness
        );
    }
    for (const auto &pos : poss) {
        draw_list->AddCircleFilled({ canvas_pos_ul.x + pos.x(), canvas_pos_br.y - pos.y() }, 5.0f, point_col);
    }
}

bool Initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, "hw1", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to load glad" << std::endl;
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, width, height);
    std::cout << "GL_VERSION: " << glGetString(GL_VERSION) <<  std::endl;
    std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) <<  std::endl; // opengl 的供应商(vendor), 我这里是 NVIDIA
    std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) <<  std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;

    // 设置主题颜色, 有以下选择, 不填的话默认是Drak
    // ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    // ImGui::StyleColorsLight();
    

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return true;
}

void BeginFrame() {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void Finalize() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    if (!Initialize()) {
        return -1;
    }

    vector<Vector2f> in_pos;
    struct {
        vector<Vector2f> pos;
        bool visible = false;
    } inter_poly;
    struct {
        vector<Vector2f> pos;
        int m = 0;
        int m_temp = 0;
        float sigma2 = 10000.0f;
        float sigma2_temp = 10000.0f;
        bool visible = false;
        bool update = false;
    } inter_gauss;
    struct {
        vector<Vector2f> pos;
        int m = 0;
        int m_temp = 0;
        bool visible = false;
        bool update = false;
    } approx_poly;
    struct {
        vector<Vector2f> pos;
        int m = 0;
        int m_temp = 0;
        float lambda = 10.0f;
        float lambda_temp = 10.0f;
        bool visible = false;
        bool update = false;
    } approx_norm;

    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig font_config;
    font_config.SizePixels = 20.0f;
    io.Fonts->AddFontDefault(&font_config);

    while (!glfwWindowShouldClose(window)) {
        BeginFrame();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        {
            ImGui::Begin("Config");
            ImGui::Checkbox("Interpolation - Polynomial (Red)", &inter_poly.visible);
            ImGui::Checkbox("Interpolation - Gauss (Green)", &inter_gauss.visible);
            ImGui::SameLine();
            ImGui::PushItemWidth(150.0f);
            ImGui::InputInt("m##1", &inter_gauss.m_temp);
            inter_gauss.m_temp = std::clamp(inter_gauss.m_temp, 0, std::max<int>(0, inter_gauss.pos.size() - 1));
            if (inter_gauss.m_temp != inter_gauss.m) {
                inter_gauss.m = inter_gauss.m_temp;
                inter_gauss.update = true;
            }
            ImGui::SameLine();
            ImGui::InputFloat("sigma2", &inter_gauss.sigma2_temp);
            inter_gauss.sigma2_temp = std::max(inter_gauss.sigma2_temp, 1.0f);
            if (inter_gauss.sigma2_temp != inter_gauss.sigma2) {
                inter_gauss.sigma2 = inter_gauss.sigma2_temp;
                inter_gauss.update = true;
            }

            ImGui::Checkbox("Fitting - Polygon (Blue)", &approx_poly.visible);
            ImGui::SameLine();
            ImGui::InputInt("m##2", &approx_poly.m_temp);
            approx_poly.m_temp = std::clamp(approx_poly.m_temp, 0, std::max<int>(0, approx_poly.pos.size() - 1));
            if (approx_poly.m_temp != approx_poly.m) {
                approx_poly.m = approx_poly.m_temp;
                approx_poly.update = true;
            }

            ImGui::Checkbox("Fitting - Normalized (Yellow)", &approx_norm.visible);
            ImGui::SameLine();
            ImGui::InputInt("m##3", &approx_norm.m_temp);
            approx_norm.m_temp = std::clamp(approx_norm.m_temp, 0, std::max<int>(0, inter_gauss.pos.size() - 1));
            if (approx_norm.m_temp != approx_norm.m) {
                approx_norm.m = approx_norm.m_temp;
                approx_norm.update = true;
            }
            ImGui::SameLine();
            ImGui::InputFloat("lambda", &approx_norm.lambda_temp);
            approx_norm.lambda_temp = std::max(approx_norm.lambda_temp, 0.0f);
            if (approx_norm.lambda_temp != approx_norm.lambda) {
                approx_norm.lambda = approx_norm.lambda_temp;
                approx_norm.update = true;
            }

            ImGui::PopItemWidth();
            ImGui::End();
        }

        {
            ImGui::Begin("Canvas");
            canvas_pos_ul = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();
            if (canvas_size.x < 50.0f) {
                canvas_size.x = 50.0f;
            }
            if (canvas_size.y < 50.0f) {
                canvas_size.y = 50.0f;
            }
            canvas_pos_br = ImVec2(canvas_pos_ul.x + canvas_size.x, canvas_pos_ul.y + canvas_size.y);

            ImGuiIO &io = ImGui::GetIO();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_pos_ul, canvas_pos_br, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_pos_ul, canvas_pos_br, IM_COL32(255, 255, 255, 255));

            float step = 20.0f;
            float lb = step; // left boundary, right boundary
            float rb = canvas_pos_br.x - step - canvas_pos_ul.x;
            ImGui::InvisibleButton("canvas", canvas_size);
            const bool is_hovered = ImGui::IsItemHovered();
            if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                in_pos.emplace_back(io.MousePos.x - canvas_pos_ul.x, canvas_pos_br.y - io.MousePos.y);
                std::sort(in_pos.begin(), in_pos.end(), [](const Eigen::Vector2f &a, const Eigen::Vector2f &b) {
                    return a.x() < b.x();
                });

                inter_poly.pos = zmath::InterpolationPolynomial(in_pos, lb, rb, step);
                inter_gauss.update = true;
                approx_poly.update = true;
                approx_norm.update = true;
            } else if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                // 右键删除鼠标足够近的一个点
                Eigen::Vector2f pos(io.MousePos.x - canvas_pos_ul.x, canvas_pos_br.y - io.MousePos.y);
                size_t index = 0;
                float min_dist = std::numeric_limits<float>::max();
                for (size_t i = 0; i < in_pos.size(); i++) {
                    float dist = (pos - in_pos[i]).squaredNorm();
                    if (dist < min_dist) {
                        min_dist = dist;
                        index = i;
                    }
                }
                if (min_dist <= 100.0f) {
                    in_pos.erase(in_pos.begin() + index);
                    inter_poly.pos = zmath::InterpolationPolynomial(in_pos, lb, rb, step);
                    inter_gauss.update = true;
                    approx_poly.update = true;
                    approx_norm.update = true;
                }
            }
            if (inter_gauss.update) {
                inter_gauss.pos = zmath::InterpolationGauss(in_pos, inter_gauss.sigma2, inter_gauss.m, lb, rb, step);
                inter_gauss.update = false;
            }
            if (approx_poly.update) {
                approx_poly.pos = zmath::ApproximationPolygon(in_pos, approx_poly.m, lb, rb, step);
                approx_poly.update = false;
            }
            if (approx_norm.update) {
                approx_norm.pos = zmath::ApproximationNormalized(in_pos, approx_norm.m, approx_norm.lambda,
                    lb, rb, step);
                approx_norm.update = false;
            }

            if (inter_poly.visible) {
                PlotLineSegments(inter_poly.pos, draw_list, IM_COL32(255, 50, 50, 255), IM_COL32(255, 80, 80, 255));
            }
            if (inter_gauss.visible) {
                PlotLineSegments(inter_gauss.pos, draw_list, IM_COL32(50, 255, 50, 255), IM_COL32(80, 255, 80, 255));
            }
            if (approx_poly.visible) {
                PlotLineSegments(approx_poly.pos, draw_list, IM_COL32(50, 50, 255, 255), IM_COL32(80, 80, 255, 255));
            }
            if (approx_norm.visible) {
                PlotLineSegments(approx_norm.pos, draw_list, IM_COL32(255, 255, 50, 255), IM_COL32(255, 255, 80, 255));
            }
            PlotLineSegments(in_pos, draw_list, IM_COL32(255, 255, 255, 0), IM_COL32(255, 255, 255, 255));

            ImGui::End();
        }

        EndFrame();
    }
    
    Finalize();
    return 0;
}

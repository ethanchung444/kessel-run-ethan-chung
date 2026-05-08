#pragma once

#include <vector>
// OpenGL Extension Wrangler - declares functions used to find OpenGL function declarations
#include <GL/glew.h>

// GLFW - declares functions used for the GLFW window manager
#include <GLFW/glfw3.h>

// ImGui - declares functions used for the ImGui user interface
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// ImPlot - used to generate plots in ImGui
#include <implot.h>
#include <implot_internal.h>

// used to time events (currently only the main loop is timed)
#include <chrono>

extern std::vector<float> gx;
extern std::vector<float> gy;
extern std::vector<float> m;
extern std::vector< std::vector<float> > px;
extern std::vector< std::vector<float> > py;


void ImGuiRender();
void DrawOutputImage();
void UpdateOutputTexture();
void PlotPaths(std::vector<float>& gx, std::vector<float>& gy,
    std::vector< std::vector<float> >& x, std::vector< std::vector<float> >& y);

void run_kessel_simulations(int num_runs = 200);

extern float shortest_path_length;
extern float longest_path_length;
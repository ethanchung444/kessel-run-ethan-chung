#include "kessel.h"

// This variable stores the OpenGL ID for the texture used to display the output image on the screen
GLuint output_image_tex = 0;


void PlotPaths(std::vector<float>& gx, std::vector<float>& gy,
    std::vector< std::vector<float> >& x, std::vector< std::vector<float> >& y) {

    ImGui::Begin("Plotted Courses");

    // Display path lengths in parsecs
    if (shortest_path_length < 1e30f) {
        ImGui::Text("Shortest Path: %.3f parsecs", shortest_path_length);
    }
    else {
        ImGui::Text("Shortest Path: --");
    }
    if (longest_path_length > 0.0f) {
        ImGui::Text("Longest Path: %.3f parsecs", longest_path_length);
    }
    else {
        ImGui::Text("Longest Path: --");
    }
    ImGui::Separator();

    if (ImPlot::BeginPlot("My Plot")) {

        // Gravity wells (red squares)
        ImPlot::PlotScatter("Gravity Wells", &gx[0], &gy[0], static_cast<int>(gx.size()),
            {
                ImPlotProp_Marker, ImPlotMarker_Square,
                ImPlotProp_MarkerSize, 10,
                ImPlotProp_LineColor, ImPlot::GetColormapColor(1),
                ImPlotProp_FillColor, ImPlot::GetColormapColor(1),
                ImPlotProp_FillAlpha, 0.25f
            });

        // Shortest path (first path in the vector)
        if (x.size() > 0) {
            ImPlot::PlotLine("Shortest Path", &x[0][0], &y[0][0], static_cast<int>(x[0].size()),
                {
                    ImPlotProp_Marker, ImPlotMarker_Circle,
                    ImPlotProp_MarkerSize, 4,
                    ImPlotProp_LineColor, ImPlot::GetColormapColor(0),  // blue
                    ImPlotProp_FillColor, ImPlot::GetColormapColor(0),
                    ImPlotProp_FillAlpha, 0.3f
                });
        }

        // Longest path (second path in the vector)
        if (x.size() > 1) {
            ImPlot::PlotLine("Longest Path", &x[1][0], &y[1][0], static_cast<int>(x[1].size()),
                {
                    ImPlotProp_Marker, ImPlotMarker_Circle,
                    ImPlotProp_MarkerSize, 4,
                    ImPlotProp_LineColor, ImPlot::GetColormapColor(2),  // green
                    ImPlotProp_FillColor, ImPlot::GetColormapColor(2),
                    ImPlotProp_FillAlpha, 0.3f
                });
        }
    }
    ImPlot::EndPlot();

    ImGui::End();
}
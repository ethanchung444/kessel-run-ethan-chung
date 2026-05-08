#include "kessel.h"
#include <random>
#include <cmath>
#include <limits>
#include <iostream>

extern std::vector<float> gx;
extern std::vector<float> gy;
extern std::vector<float> m;
extern std::vector< std::vector<float> > px;
extern std::vector< std::vector<float> > py;
float shortest_path_length = std::numeric_limits<float>::infinity();
float longest_path_length = -1.0f;

void run_kessel_simulations(int num_runs) {
    px.clear();
    py.clear();

    std::vector<float> shortest_x, shortest_y;
    std::vector<float> longest_x, longest_y;
    float shortest_len = std::numeric_limits<float>::infinity();
    float longest_len = -1.0f;
    int success_count = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> px_dist(-5.0f, 5.0f);
    std::uniform_real_distribution<float> speed_dist(2.0f, 5.0f);
    std::normal_distribution<float> theta_dist(1.5708f, 0.7854f);

    const float G = 1.0f;
    const float dt = 0.001f;
    const float MAX_A = 4.0f;
    const float PI = 3.1415926535f;

    for (int run = 0; run < num_runs; ++run) {
        float x = px_dist(gen);
        float y = -10.0f;
        float speed = speed_dist(gen);
        float theta = theta_dist(gen);
        if (theta < PI / 4.0f) theta = PI / 4.0f;
        if (theta > 3.0f * PI / 4.0f) theta = 3.0f * PI / 4.0f;

        float vx = speed * std::cos(theta);
        float vy = speed * std::sin(theta);

        std::vector<float> path_x = { x };
        std::vector<float> path_y = { y };

        float path_length = 0.0f;
        bool success = false;

        for (int step = 0; step < 200000; ++step) {
            float ax = 0.0f;
            float ay = 0.0f;

            for (size_t i = 0; i < gx.size(); ++i) {
                float dx = gx[i] - x;
                float dy = gy[i] - y;
                float r2 = dx * dx + dy * dy;
                if (r2 < 1e-8f) continue;
                float r3 = r2 * std::sqrt(r2);
                float f = G * m[i] * 2e-22f / r3; //scale the force so that the ship doesn't crash in high gravity well volume simulations (80+)
                ax += f * dx;
                ay += f * dy;
            }

            float a_mag = std::sqrt(ax * ax + ay * ay);
            if (a_mag > MAX_A) break;

            vx += ax * dt;
            vy += ay * dt;

            float new_x = x + vx * dt;
            float new_y = y + vy * dt;

            float dxp = new_x - x;
            float dyp = new_y - y;
            path_length += std::sqrt(dxp * dxp + dyp * dyp);

            x = new_x;
            y = new_y;

            path_x.push_back(x);
            path_y.push_back(y);

            if (y > 10.0f) {
                success = true;
                success_count++;
                break;
            }
            if (x < -10.0f || x > 10.0f) break;
        }

        if (success) {
            if (path_length < shortest_len) {
                shortest_len = path_length;
                shortest_x = path_x;
                shortest_y = path_y;
            }
            if (path_length > longest_len) {
                longest_len = path_length;
                longest_x = path_x;
                longest_y = path_y;
            }
        }
    }

    // Store best paths
    if (!shortest_x.empty()) {
        px.push_back(shortest_x);
        py.push_back(shortest_y);
    }
    if (!longest_x.empty()) {
        px.push_back(longest_x);
        py.push_back(longest_y);
    }

    // Store lengths globally so the GUI can display them
    shortest_path_length = shortest_len;
    longest_path_length = longest_len;

    // Fallback
    if (px.empty()) {
        std::vector<float> dummy_x = { 0.0f, 0.0f };
        std::vector<float> dummy_y = { -10.0f, 10.0f };
        px.push_back(dummy_x);
        py.push_back(dummy_y);
    }

    std::cout << "Kessel Run simulations completed (" << num_runs << " runs).\n";
    std::cout << "Successful runs: " << success_count << "\n";
    std::cout << "Shortest successful path length: " << shortest_len << "\n";
    std::cout << "Longest successful path length: " << longest_len << std::endl;
}
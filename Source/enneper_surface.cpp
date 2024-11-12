#include "enneper_surface.h"
#include <vector>
#include <cmath>

EnneperSurface& EnneperSurface::Instance() {
    static EnneperSurface instance;
    return instance;
}

EnneperSurface::EnneperSurface() {
    // Parameters for the surface
    float u_start = -2.0f;
    float u_end = 2.0f;
    float v_start = -2.0f;
    float v_end = 2.0f;
    int N = 50; // Grid resolution

    float du = (u_end - u_start) / (N - 1);
    float dv = (v_end - v_start) / (N - 1);

    std::vector<float> vertices;
    vertices.reserve(N * N * 7); // 7 floats per vertex (position + color)

    // Generate vertices
    for (int i = 0; i < N; ++i) {
        float u = u_start + i * du;
        for (int j = 0; j < N; ++j) {
            float v = v_start + j * dv;

            float u2 = u * u;
            float u3 = u2 * u;
            float v2 = v * v;
            float v3 = v2 * v;

            float x = u - u3 / 3.0f + u * v2;
            float y = v - v3 / 3.0f + v * u2;
            float z = u2 - v2;

            // Normalize coordinates for color mapping
            float r = (x - (-4.0f)) / (8.0f); // Assuming x varies roughly from -4 to 4
            float g = (y - (-4.0f)) / (8.0f); // Assuming y varies roughly from -4 to 4
            float b = (z - (-4.0f)) / (8.0f); // Assuming z varies roughly from -4 to 4
            float a = 1.0f;

            // Clamp color values between 0 and 1
            r = fmaxf(0.0f, fminf(1.0f, r));
            g = fmaxf(0.0f, fminf(1.0f, g));
            b = fmaxf(0.0f, fminf(1.0f, b));

            // Push back position and color
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
            vertices.push_back(a);
        }
    }

    // Generate indices for triangle strips
    std::vector<uint16_t> indices;
    indices.reserve((N - 1) * (N - 1) * 6); // 6 indices per quad (2 triangles)

    for (int i = 0; i < N - 1; ++i) {
        for (int j = 0; j < N - 1; ++j) {
            // Indices of the four corners
            uint16_t i0 = i * N + j;
            uint16_t i1 = i * N + (j + 1);
            uint16_t i2 = (i + 1) * N + (j + 1);
            uint16_t i3 = (i + 1) * N + j;

            // First triangle (i0, i1, i2)
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            // Second triangle (i0, i2, i3)
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    index_count = static_cast<int>(indices.size());

    // Create vertex buffer
    vertex_buffer = sg_make_buffer({
        .data = SG_RANGE(vertices),
        .label = "enneper-vertices",
        });

    // Create index buffer
    sg_buffer_desc index_buffer_desc = {
        .data = SG_RANGE(indices),
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .label = "enneper-indices",
    };

    index_buffer = sg_make_buffer(&index_buffer_desc);

    // Set up bindings
    bindings = {};
    bindings.vertex_buffers[0] = vertex_buffer;
    bindings.index_buffer = index_buffer;
}

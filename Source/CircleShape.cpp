#include "CircleShape.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#define PI 3.14159265358979323846

CircleShape& CircleShape::Instance() {
    static CircleShape instance;
    return instance;
}

CircleShape::CircleShape() {
    // Number of points for the circle
    const int N = 100;  // Adjust for smoothness

    std::vector<float> vertices;
    vertices.reserve(N * 7); // Each vertex has 7 floats (position + color)

    for (int i = 0; i < N; ++i) {
        float t = (float)i / (float)(N - 1) * 2.0f * PI;

        // Calculate position
        float x = cosf(t);
        float y = sinf(t);
        float z = 0.0f;

        // Calculate color (optional, can be customized)
        float r = (sinf(t) + 1.0f) * 0.5f;
        float g = (cosf(t) + 1.0f) * 0.5f;
        float b = 0.5f;
        float a = 1.0f;

        // Add position
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);

        // Add color
        vertices.push_back(r);
        vertices.push_back(g);
        vertices.push_back(b);
        vertices.push_back(a);
    }

    vertex_count = N;








    glm::mat4 identity_matrix = glm::mat4(1.0f);

    // Step 2: Create a buffer for the identity matrix
    sg_buffer_desc mat_buffer_desc = {
        .data = {
            .ptr = &identity_matrix,
            .size = sizeof(glm::mat4),
        },
        .usage = SG_USAGE_IMMUTABLE, // The matrix won't change
        .label = "identity-matrix-buffer",
    };
    sg_buffer matrix_buffer = sg_make_buffer(&mat_buffer_desc);





    // Create the vertex buffer
    sg_buffer_desc buffer_desc = {
        .data = {
            .ptr = vertices.data(),
            .size = vertices.size() * sizeof(float)
        },
        .label = "circle-vertices",
    };

    vertex_buffer = sg_make_buffer(&buffer_desc);

    bindings = {
        .vertex_buffers[0] = vertex_buffer,
        .vertex_buffers[1] = matrix_buffer,
    };
}

void CircleShape::release() {
    sg_destroy_buffer(vertex_buffer);
}

#include "CircleShape.h"
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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





    transform_matrices.push_back(glm::mat4(1.0f));
    instance_count = 1;

    // Create the instance buffer with dynamic usage
    sg_buffer_desc instance_buffer_desc = {
        .size = sizeof(glm::mat4),
        .usage = SG_USAGE_DYNAMIC, // The matrix can change
        .label = "circle-instance-buffer",
    };
    instance_buffer = sg_make_buffer(&instance_buffer_desc);

    // Update the instance buffer with the initial transform

    sg_update_buffer(instance_buffer, (sg_range) {
        .ptr = transform_matrices.data(),
            .size = sizeof(glm::mat4)
    });







    // Create the vertex buffer
    sg_buffer_desc buffer_desc = {
        .data = {
            .ptr = vertices.data(),
            .size = vertices.size() * sizeof(float)
        },
        .label = "circle-vertices",
    };

    vertex_buffer = sg_make_buffer(&buffer_desc);

    // Set up the bindings
    bindings = {
        .vertex_buffers[0] = vertex_buffer,
        .vertex_buffers[1] = instance_buffer,
    };
}


void CircleShape::SetTranslation(const glm::vec3& translation) {
    // Update the transformation matrix
    transform_matrices[0] = glm::translate(glm::mat4(1.0f), translation);

    // Update the instance buffer with the new matrix
    sg_update_buffer(instance_buffer, (sg_range) {
        .ptr = transform_matrices.data(),
            .size = sizeof(glm::mat4)
    });
}




void CircleShape::SetTranslations(const std::vector<glm::vec3>& translations, const std::vector<glm::vec3>& scales) {
    // Ensure we have the same number of translations and scales
    assert(translations.size() == scales.size() && "Translations and scales must have the same size!");

    instance_count = static_cast<int>(translations.size());
    transform_matrices.resize(instance_count);

    for (size_t i = 0; i < translations.size(); ++i) {
        // Combine translation and scaling
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scales[i]);
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translations[i]);
        transform_matrices[i] = translation_matrix * scale_matrix;
    }

    // Resize the instance buffer if necessary
    size_t buffer_size = sizeof(glm::mat4) * instance_count;

    // Recreate the instance buffer
    sg_buffer_desc instance_buffer_desc = {
        .size = buffer_size,
        .usage = SG_USAGE_DYNAMIC,
        .label = "circle-instance-buffer",
    };
    sg_destroy_buffer(instance_buffer);
    instance_buffer = sg_make_buffer(&instance_buffer_desc);

    // Update the instance buffer with the transforms
    sg_update_buffer(instance_buffer, (sg_range) {
        .ptr = transform_matrices.data(),
            .size = buffer_size
    });

    // Update bindings
    bindings.vertex_buffers[1] = instance_buffer;
}




void CircleShape::release() {
    sg_destroy_buffer(vertex_buffer);
}

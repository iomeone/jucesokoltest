#include "quad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


SimpleQuad& SimpleQuad::Instance() {
    static SimpleQuad instance;
    return instance;
}

SimpleQuad::SimpleQuad() {
    float vertices[] = {
        // positions            // colors
         0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f
    };

    vertex_buffer = sg_make_buffer({
        .data = SG_RANGE(vertices),
        .label = "quad-vertices",
        });




    //#include <glm/gtc/matrix_transform.hpp>
    // Step 1: Define the transforms for the three quads
    glm::mat4 transforms[3] = {
        glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f)), // Quad at x = -0.5
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), // Quad at x = 0.0
        glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f))  // Quad at x = 0.5
    };

    // Step 2: Create an instance buffer for the transforms
    sg_buffer_desc instance_buffer_desc = {
        .data = {
            .ptr = transforms,
            .size = sizeof(transforms),
        },
        .usage = SG_USAGE_IMMUTABLE, // The transforms won't change
        .label = "instance-transform-buffer",
    };
    sg_buffer instance_buffer = sg_make_buffer(&instance_buffer_desc);

    // Step 3: Set up the vertex and instance buffers in sg_bindings
    bindings = (sg_bindings){
        .vertex_buffers[0] = vertex_buffer,      // Per-vertex data (position and color)
        .vertex_buffers[1] = instance_buffer,   // Per-instance transform data
        .index_buffer = index_buffer,           // Index buffer
    };




    {
        uint16_t indices[] = {
                0, 1, 2 // Triangle
            };



        index_buffer = sg_make_buffer({
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .data = SG_RANGE(indices),
            .label = "quad-indices",
            });

        bindings_use_index = (sg_bindings){
            .vertex_buffers[0] = vertex_buffer,
            .vertex_buffers[1] = instance_buffer,
            .index_buffer = index_buffer,
        };

    }

}

void SimpleQuad::release() {
    sg_destroy_buffer(vertex_buffer);
}
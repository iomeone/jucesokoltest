#include "quad.h"


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


    {
        bindings = (sg_bindings){
                .vertex_buffers[0] = vertex_buffer
                };
    }



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
            .index_buffer = index_buffer,
        };

    }

}

void SimpleQuad::release() {
    sg_destroy_buffer(vertex_buffer);
}
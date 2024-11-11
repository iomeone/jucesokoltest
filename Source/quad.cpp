#include "quad.h"


SimpleQuad& SimpleQuad::Instance() {
    static SimpleQuad instance;  // Î¨Ò»µÄÊµÀý
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

    bindings = (sg_bindings){
        .vertex_buffers[0] = vertex_buffer
    };
}
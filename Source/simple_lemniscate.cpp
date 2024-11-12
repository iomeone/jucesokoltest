#include "simple_lemniscate.h"
#include <vector>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

SimpleLemniscate& SimpleLemniscate::Instance() {
    static SimpleLemniscate instance;   
    return instance;
}

SimpleLemniscate::SimpleLemniscate() {
 
    const int u_steps = 50; 
    const int v_steps = 50; 

    std::vector<float> vertices;
    std::vector<uint16_t> indices;

 
    for (int i = 0; i <= u_steps; ++i) {
        float u = M_PI * i / (float)u_steps; // 0 <= u <= pi
        for (int j = 0; j <= v_steps; ++j) {
            float v = M_PI * j / (float)v_steps; // 0 <= v <= pi

            float cos_v = cosf(v);
            float sin_2u = sinf(2 * u);
            float sqrt_sin_2u = sqrtf(fabsf(sin_2u));

            float cos_u = cosf(u);
            float sin_u = sinf(u);
            float x = cos_v * sqrt_sin_2u * cos_u;
            float y = cos_v * sqrt_sin_2u * sin_u;

            float tan_v = tanf(v);
            float tan_v_squared = tan_v * tan_v;

            float z = x * x - y * y + 2 * x * y * tan_v_squared;
   

         
            float r = (x + 1.0f) * 0.5f; //[-1,1] to [0,1]
            float g = (y + 1.0f) * 0.5f;
            float b = (z + 1.0f) * 0.5f;
            float a = 1.0f;

           
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
            vertices.push_back(a);
        }
    }

 
    for (int i = 0; i < u_steps; ++i) {
        for (int j = 0; j < v_steps; ++j) {
            uint16_t index0 = i * (v_steps + 1) + j;
            uint16_t index1 = index0 + 1;
            uint16_t index2 = index0 + (v_steps + 1);
            uint16_t index3 = index2 + 1;

            indices.push_back(index0);
            indices.push_back(index2);
            indices.push_back(index1);

          
            indices.push_back(index1);
            indices.push_back(index2);
            indices.push_back(index3);
        }
    }

    num_elements = static_cast<int>(indices.size());

    vertex_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_VERTEXBUFFER,
        .data = {
            .ptr = vertices.data(),
            .size = vertices.size() * sizeof(float)
        },
        .label = "lemniscate-vertices",
        });

    index_buffer = sg_make_buffer({
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = {
            .ptr = indices.data(),
            .size = indices.size() * sizeof(uint16_t)
        },
        .label = "lemniscate-indices",
        });

    bindings = (sg_bindings){
        .vertex_buffers[0] = vertex_buffer,
        .index_buffer = index_buffer,
    };
}



//call in _sg_shutdown
void SimpleLemniscate::release() {
    sg_destroy_buffer(vertex_buffer);
    sg_destroy_buffer(index_buffer);
}
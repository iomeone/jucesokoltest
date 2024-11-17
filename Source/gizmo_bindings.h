#pragma once
#include "sokol_gfx.h"
#include <vector>

#include "common.h"


struct geometry_vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
};



class GizmoBindings {
public:
    GizmoBindings();
    void release();

    void Update(const std::vector<geometry_vertex>& vertices, const std::vector<uint32_t>& indices);

    sg_bindings bindings;
private:
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
};

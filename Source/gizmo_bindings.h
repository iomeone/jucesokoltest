#pragma once
#include "sokol_gfx.h"
#include <vector>

#include "common.h"
#include "tiny-gizmo.hpp"

//struct geometry_vertex {
//    glm::vec3 position;
//    glm::vec3 normal;
//    glm::vec4 color;
//};



class GizmoBindings {
public:
    GizmoBindings();
    void release();

    void GizmoBindings::Update(const std::vector<tinygizmo::geometry_vertex>& vertices, const std::vector<minalg::uint3>& triangles);

    size_t GetNumElements() const { return index_count; }

    sg_bindings bindings{ 0 };
private:
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;

    size_t index_count;
};

#include "gizmo_bindings.h"

#define MAX_GIZMO_VERTICES 65536
#define MAX_GIZMO_INDICES 65536

GizmoBindings::GizmoBindings() : index_count(0) {
    // 创建顶点缓冲区
    vertex_buffer = sg_make_buffer((sg_buffer_desc) {
        .usage = SG_USAGE_DYNAMIC,
            .type = SG_BUFFERTYPE_VERTEXBUFFER,
            .size = sizeof(tinygizmo::geometry_vertex) * MAX_GIZMO_VERTICES,
            .label = "gizmo-vertex-buffer"
    });

    assert(vertex_buffer.id != SG_INVALID_ID);

    // 创建索引缓冲区
    index_buffer = sg_make_buffer((sg_buffer_desc) {
        .usage = SG_USAGE_DYNAMIC,
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .size = sizeof(uint32_t) * MAX_GIZMO_INDICES,
            .label = "gizmo-index-buffer"
    });

    assert(index_buffer.id != SG_INVALID_ID);

    bindings.vertex_buffers[0] = vertex_buffer;
    bindings.index_buffer = index_buffer;
}

void GizmoBindings::release() {
    sg_destroy_buffer(vertex_buffer);
    sg_destroy_buffer(index_buffer);
}




void GizmoBindings::Update(const std::vector<tinygizmo::geometry_vertex>& vertices, const std::vector<minalg::uint3>& triangles) {
    // 更新顶点缓冲区
    sg_update_buffer(vertex_buffer, (sg_range){
        .ptr = vertices.data(),
            .size = sizeof(tinygizmo::geometry_vertex) * vertices.size()
    });

    //// 将 triangles 展开为平面的索引数组
    //std::vector<uint32_t> indices;
    //indices.reserve(triangles.size() * 3);
    //for (const auto& tri : triangles) {
    //    indices.push_back(tri.x);
    //    indices.push_back(tri.y);
    //    indices.push_back(tri.z);
    //}

    // 更新索引缓冲区
    sg_update_buffer(index_buffer, (sg_range){
        .ptr = triangles.data(),
            .size = sizeof(minalg::uint3) * triangles.size()
    });

    // 保存索引数以便绘制时使用
    index_count = triangles.size()*3;
}


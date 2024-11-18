#include "gizmo_bindings.h"

#define MAX_GIZMO_VERTICES 65536
#define MAX_GIZMO_INDICES 65536

GizmoBindings::GizmoBindings() : index_count(0) {
    // ´´½¨¶¥µã»º³åÇø
    vertex_buffer = sg_make_buffer((sg_buffer_desc) {
        .usage = SG_USAGE_DYNAMIC,
            .type = SG_BUFFERTYPE_VERTEXBUFFER,
            .size = sizeof(tinygizmo::geometry_vertex) * MAX_GIZMO_VERTICES,
            .label = "gizmo-vertex-buffer"
    });

    assert(vertex_buffer.id != SG_INVALID_ID);

    // ´´½¨Ë÷Òý»º³åÇø
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
    // ¸üÐÂ¶¥µã»º³åÇø
    sg_update_buffer(vertex_buffer, (sg_range){
        .ptr = vertices.data(),
            .size = sizeof(tinygizmo::geometry_vertex) * vertices.size()
    });

    //// ½« triangles Õ¹¿ªÎªÆ½ÃæµÄË÷ÒýÊý×é
    //std::vector<uint32_t> indices;
    //indices.reserve(triangles.size() * 3);
    //for (const auto& tri : triangles) {
    //    indices.push_back(tri.x);
    //    indices.push_back(tri.y);
    //    indices.push_back(tri.z);
    //}

    // ¸üÐÂË÷Òý»º³åÇø
    sg_update_buffer(index_buffer, (sg_range){
        .ptr = triangles.data(),
            .size = sizeof(minalg::uint3) * triangles.size()
    });

    // ±£´æË÷ÒýÊýÒÔ±ã»æÖÆÊ±Ê¹ÓÃ
    index_count = triangles.size()*3;
}


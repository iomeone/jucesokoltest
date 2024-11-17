#include "gizmo_bindings.h"

#define MAX_GIZMO_VERTICES 65536
#define MAX_GIZMO_INDICES 65536

GizmoBindings::GizmoBindings() {
    // �������㻺����
    vertex_buffer = sg_make_buffer((sg_buffer_desc) {
        .usage = SG_USAGE_STREAM,
            .type = SG_BUFFERTYPE_VERTEXBUFFER,
            .size = sizeof(geometry_vertex) * MAX_GIZMO_VERTICES,
            .label = "gizmo-vertex-buffer"
    });

    // ��������������
    index_buffer = sg_make_buffer((sg_buffer_desc) {
        .usage = SG_USAGE_STREAM,
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .size = sizeof(uint32_t) * MAX_GIZMO_INDICES,
            .label = "gizmo-index-buffer"
    });

    bindings.vertex_buffers[0] = vertex_buffer;
    bindings.index_buffer = index_buffer;
}

void GizmoBindings::release() {
    sg_destroy_buffer(vertex_buffer);
    sg_destroy_buffer(index_buffer);
}

void GizmoBindings::Update(const std::vector<geometry_vertex>& vertices, const std::vector<uint32_t>& indices) {
    // ���¶��㻺����
    sg_update_buffer(vertex_buffer, (sg_range){
        .ptr = vertices.data(),
        .size = sizeof(geometry_vertex) * vertices.size()
    });

    // ��������������
    sg_update_buffer(index_buffer, (sg_range){
        .ptr = indices.data(),
        .size = sizeof(uint32_t) * indices.size()
    });
}

#include "gizmo_pipeline.h"




const char gizmo_vs[] = R"(
    #version 330
    layout(location = 0) in vec3 in_position;
    layout(location = 1) in vec3 in_normal;
    layout(location = 2) in vec4 in_color;

    uniform mat4 u_mvp;

    out vec4 v_color;

    void main() {
        gl_Position = u_mvp * vec4(in_position, 1.0);
        v_color = in_color;
    }
)";

const char gizmo_fs[] = R"(
    #version 330
    in vec4 v_color;
    out vec4 frag_color;

    void main() {
        frag_color = v_color;
    }
)";



GizmoPipeline::GizmoPipeline() {
    // 创建着色器
    shader = sg_make_shader((sg_shader_desc) {
        .vs = {
            .source = gizmo_vs,
            .uniform_blocks[0] = {
                .size = sizeof(gizmo_vs_params_t),
                .uniforms = {
                    [0] = {.name = "u_mvp", .type = SG_UNIFORMTYPE_MAT4 }
                }
            }
        },
            .fs = {
                .source = gizmo_fs
        },
            .label = "gizmo-shader"
    });

    // 创建管线
    pipeline = sg_make_pipeline((sg_pipeline_desc) {
        .layout = {
            .attrs = {
                [0] = {.format = SG_VERTEXFORMAT_FLOAT3 }, // in_position
                [1] = {.format = SG_VERTEXFORMAT_FLOAT3 }, // in_normal
                [2] = {.format = SG_VERTEXFORMAT_FLOAT4 }  // in_color
            }
        },
            .shader = shader,
            .index_type = SG_INDEXTYPE_UINT32,
            .depth = {
                .compare = SG_COMPAREFUNC_LESS_EQUAL,
                .write_enabled = true
        },
            .cull_mode = SG_CULLMODE_BACK,
            .label = "gizmo-pipeline"
    });
}

void GizmoPipeline::release() {
    sg_destroy_pipeline(pipeline);
    sg_destroy_shader(shader);
}

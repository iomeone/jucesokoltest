#include "pos_color_pipeline.h"




const char simple_quad_vs[] = R"(#version 300 es
        // attributes
        layout(location = 0) in vec4 in_position;
        layout(location = 1) in vec4 in_color;

        out vec4 color;

        void main()
        {
            gl_Position = in_position;
            color = in_color;
        })";
const char simple_quad_fs[] = R"(#version 300 es
        precision mediump float;
        in vec4 color;
        out vec4 FragColor;
        void main()
        {
            FragColor = color;
        })";



SimplePipeline::SimplePipeline(sg_primitive_type primitive_type)
{
    pipeline = sg_make_pipeline((sg_pipeline_desc) {
        .layout = {
            .attrs = {
                [0] = {
                    .format = SG_VERTEXFORMAT_FLOAT3,
                    .buffer_index = 0,
                    .offset = 0,
                },
                [1] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 0,
                    .offset = sizeof(float) * 3,  // offset after the position data
                },
            },
        },

        .primitive_type = primitive_type,

        .shader = sg_make_shader((sg_shader_desc) {
            .vs =
            {
                .source = simple_quad_vs,
            },
            .fs =
            {
                .source = simple_quad_fs,
            }
        }),

        .label = "simple-quad-pipeline",
    });


    pipeline_use_index = sg_make_pipeline((sg_pipeline_desc) {
        .layout = {
            .attrs = {
                [0] = {
                    .format = SG_VERTEXFORMAT_FLOAT3,
                    .buffer_index = 0,
                    .offset = 0,
                },
                [1] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 0,
                    .offset = sizeof(float) * 3,  // offset after the position data
                },
            },
        },

        .index_type = SG_INDEXTYPE_UINT16,
        .primitive_type = primitive_type,
        .shader = sg_make_shader((sg_shader_desc) {
            .vs =
            {
                .source = simple_quad_vs,
            },
            .fs =
            {
                .source = simple_quad_fs,
            }
        }),

        .label = "simple-quad-pipeline",
    });
}



 
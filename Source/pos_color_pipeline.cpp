#include "pos_color_pipeline.h"




const char simple_quad_vs[] = R"(#version 300 es
        // attributes
        layout(location = 0) in vec4 in_position;
        layout(location = 1) in vec4 in_color;
        layout(location = 2) in mat4 i_mat_m;

        uniform mat4 view_proj;
        out vec4 color;

        void main()
        {
            gl_Position = view_proj * i_mat_m * in_position;
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



                [2] = {
                    .format = SG_VERTEXFORMAT_FLOAT4, 
                    .buffer_index = 1,
                    .offset = 0,
                },
                [3] = {
                    .format = SG_VERTEXFORMAT_FLOAT4, 
                    .buffer_index = 1,
                    .offset = 16,
                },
                [4] = {
                    .format = SG_VERTEXFORMAT_FLOAT4, 
                    .buffer_index = 1,
                    .offset = 32,
                },
                [5] = {
                    .format = SG_VERTEXFORMAT_FLOAT4, 
                    .buffer_index = 1,
                    .offset = 48,
                },

            },

           .buffers = {
               [0] = {
                    .stride = sizeof(float) * 7,  // 3 pos, 4 color
                    .step_func = SG_VERTEXSTEP_PER_VERTEX,
               },
               [1] = {
                   .stride = sizeof(glm::mat4),
                   .step_func = SG_VERTEXSTEP_PER_INSTANCE,
               },
           },
        },



        .primitive_type = primitive_type,

        .shader = sg_make_shader((sg_shader_desc) {
            .vs =
            {
                .source = simple_quad_vs,
                .uniform_blocks = {
                    [0] = {
                       
                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),
                        
                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            //[1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                }
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



                [2] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 1,
                    .offset = 0,
                },
                [3] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 1,
                    .offset = 16,
                },
                [4] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 1,
                    .offset = 32,
                },
                [5] = {
                    .format = SG_VERTEXFORMAT_FLOAT4,
                    .buffer_index = 1,
                    .offset = 48,
                },

            },

           .buffers = {
               [0] = {
                    .stride = sizeof(float) * 7,  // 3 pos, 4 color
                    .step_func = SG_VERTEXSTEP_PER_VERTEX,
               },
               [1] = {
                   .stride = sizeof(glm::mat4),
                   .step_func = SG_VERTEXSTEP_PER_INSTANCE,
               },
           },
        },

        .index_type = SG_INDEXTYPE_UINT16,
        .primitive_type = primitive_type,
        .shader = sg_make_shader((sg_shader_desc) {
            .vs =
            {
                .source = simple_quad_vs,

                .uniform_blocks = {
                    [0] = {

                        .layout = SG_UNIFORMLAYOUT_NATIVE,
                        .size = sizeof(vs_params_t),

                        .uniforms = {
                            [0] = {.name = "view_proj", .type = SG_UNIFORMTYPE_MAT4},
                            //[1] = {.name = "model", .type = SG_UNIFORMTYPE_MAT4},
                        },
                    },
                }
            },
            .fs =
            {
                .source = simple_quad_fs,
            }
        }),

        .label = "simple-quad-pipeline",
    });
}



 
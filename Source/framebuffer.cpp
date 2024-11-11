#include "framebuffer.h"

const char no_post_process_vs[] = R"(#version 300 es
        // attributes
        layout(location = 0) in vec2 in_position;
        layout(location = 1) in vec2 in_texcoord;
        out vec2 texcoords;
        void main()
        {
            texcoords = in_texcoord;
            gl_Position = vec4(in_position.xy, 0.0, 1.0);
        })";
const char no_post_process_fs[] = R"(#version 300 es
        precision mediump float;
        out vec4 FragColor;
        in vec2 texcoords;
        uniform sampler2D screen;
        void main()
        {
            FragColor = texture(screen, texcoords).rgba;
        })";

Framebuffer::Framebuffer(void)
{
    sg_image_desc img_desc = {
        .render_target = true,
        .width = 800,
        .height = 600,
    };

    // color attachment
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.label = "framebuffer-color-image";
    color = sg_make_image(img_desc);

    // depth attachment
    img_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    img_desc.label = "framebuffer-depth-image";
    depth = sg_make_image(img_desc);

    pass = (sg_pass){
        .action = (sg_pass_action){
            .colors[0] = {
                .clear_value = {0.0f, 0.0f, 0.0f, 0.0f},
                .load_action = SG_LOADACTION_CLEAR,
            },
        },
        .attachments = sg_make_attachments({
            .colors[0].image = color,
            .depth_stencil.image = depth,
            .label = "framebuffer-attachments",
        }),
    };

    pipeline = sg_make_pipeline((sg_pipeline_desc) {
        .layout = {
            .attrs = {
                [0] .format = SG_VERTEXFORMAT_FLOAT2,
                [1].format = SG_VERTEXFORMAT_FLOAT2,
            },
        },
        .shader = sg_make_shader((sg_shader_desc) {
            .vs = 
            {
                .source = no_post_process_vs,
            },
            .fs = 
                { 
                    .source = no_post_process_fs,
                    .images[0] = {
                            .used = true,
                            .image_type = SG_IMAGETYPE_2D,
                            .sample_type = _SG_IMAGESAMPLETYPE_DEFAULT,

            
                      },
                    .samplers[0] = {
                        .used = true,
                        .sampler_type = SG_SAMPLERTYPE_FILTERING,
            
            
                     },
                    .image_sampler_pairs[0] = {
                        .used = true,
                        .image_slot = 0,
                        .sampler_slot = 0,
                        .glsl_name = "screen",
                },
            }
        }),
        .colors[0] = {
            .blend = {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_ONE,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE,
                .op_rgb = SG_BLENDOP_ADD,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
                .op_alpha = SG_BLENDOP_ADD,
            },
        },
        .label = "framebuffer-pipeline",
    });

    // clang-format off
    float quad_vertices[] = {
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
    };
    // clang-format on

    // create buffer
    vertex_buffer = sg_make_buffer({
        .data = SG_RANGE(quad_vertices),
        .label = "framebuffer-vertices",
        });



    // apply bindings
    bindings = (sg_bindings){
        .vertex_buffers[0] = vertex_buffer,
        .fs.images[0] = color,
        .fs.samplers[0] = sg_make_sampler({
            .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
            .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
            .min_filter = SG_FILTER_LINEAR,
            .mag_filter = SG_FILTER_LINEAR,
        }),
    };
}
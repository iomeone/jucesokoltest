
#include "render.h"


//#define SOKOL_GFX_INCLUDED


#include "sokol_gfx.h"


static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;




void _sg_initialize()
{
    {
        //sg_desc desc;
        //sg_setup(&desc{0});

        sg_setup(&(sg_desc) {
            0
        });


        sg_shader shd = sg_make_shader(&(sg_shader_desc) {
            .attrs = {
                [0] .name = "position"
            },
                .vs.source =
                "#version 330\n"
                "layout(location = 0) in vec4 position;\n"
                //"attribute vec4 color0;\n"
                //"varying vec4 color;\n"
                "void main() {\n"
                "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
                //"  color = color0;\n"
                "}\n",
                .fs.source =
                "#version 330 \n"
                " layout(location = 0) out vec4 FragColor;\n"
                //"varying vec4 color;\n"
                "void main() {\n"
                "  FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                "}\n"
        });


        //vertices buffer
        {
            float vertices[] = {
                // positions
                -0.5f, -0.5f, 0.0f,     // bottom left
                0.5f, -0.5f, 0.0f,      // bottom right
                0.0f,  0.5f, 0.0f       // top
            };

            state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc) {
                .size = sizeof(vertices),
                    .data = SG_RANGE(vertices),
                    .label = "triangle-vertices"
            });

        }



        /* create a pipeline object (default render states are fine for triangle) */

        state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
            .shader = shd,
                /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
                .layout = {
                    .attrs = {
                        [0] .format = SG_VERTEXFORMAT_FLOAT3
                    }
            },
                .label = "triangle-pipeline"
        });


        state.pass_action = (sg_pass_action){
            .colors[0] = {.load_action = SG_LOADACTION_CLEAR,  .clear_value = {0.6f, 0.6f, 0.9f, 1.0f} }
        };

    }

}


void _sg_shutdown()
{
    sg_shutdown();
}


void _sg_render(int w, int h)
{



    sg_begin_pass(&(sg_pass) { .action = state.pass_action, 
                               .swapchain = (sg_swapchain) {.width = w, .height = h} });



    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    sg_end_pass();
    sg_commit();

}
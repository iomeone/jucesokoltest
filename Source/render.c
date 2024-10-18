#include "render.h"
#include "sokol_gfx.h"
#include "flecs.h"
#include "cglm.h"

static struct {
    int width;
    int height;
} state;

static ecs_world_t* world;

 

typedef struct SokolCanvas {
    sg_pass_action pass_action;
    sg_pipeline pipeline;
} SokolCanvas;

typedef struct {
    float m[16];
} Transform;

typedef struct {
    float r, g, b, a;
} Color;

typedef struct SokolBuffer {
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    sg_buffer color_buffer;
    sg_buffer transform_buffer;

    Color* colors;
    Transform* transforms;

    int32_t instance_count;
    int32_t index_count;
} SokolBuffer;


ECS_COMPONENT_DECLARE(SokolCanvas);
ECS_COMPONENT_DECLARE(SokolBuffer);

static
sg_pass_action init_pass_action(const Color bg_color)
{
    return (sg_pass_action) {
        .colors[0] = {
            .load_action = SG_LOADACTION_CLEAR,
            .clear_value = {
                bg_color.r,
                bg_color.g,
                bg_color.b,
                bg_color.a,
            }
        }
    };
}

static
sg_pipeline init_pipeline(void) {
    sg_shader shd = sg_make_shader(&(sg_shader_desc) {
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec3 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "layout(location=2) in mat4 mat;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = mat * vec4(position, 1.0);\n"
            "  color = color0;\n"
            "}\n",
            .fs.source =
            "#version 330\n"
            "in vec4 color;\n"
            "out vec4 frag_color;\n"
            "void main() {\n"
            "  frag_color = color;\n"
            "}\n"
    });

    return sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = shd,
            .index_type = SG_INDEXTYPE_UINT16,
            .layout = {
                .buffers = {
                    [0] = {.stride = sizeof(float) * 3 },
                    [1] = {.stride = sizeof(Color), .step_func = SG_VERTEXSTEP_PER_INSTANCE },
                    [2] = {.stride = sizeof(Transform), .step_func = SG_VERTEXSTEP_PER_INSTANCE }
                },
                .attrs = {
                    [0] = {.buffer_index = 0, .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT3 },
                    [1] = {.buffer_index = 1, .offset = 0, .format = SG_VERTEXFORMAT_FLOAT4 },
                    [2] = {.buffer_index = 2, .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT4 },
                    [3] = {.buffer_index = 2, .offset = 16, .format = SG_VERTEXFORMAT_FLOAT4 },
                    [4] = {.buffer_index = 2, .offset = 32, .format = SG_VERTEXFORMAT_FLOAT4 },
                    [5] = {.buffer_index = 2, .offset = 48, .format = SG_VERTEXFORMAT_FLOAT4 }
                }
        }
    });
}

static void SokolSetCanvas(ecs_iter_t* it) {
    SokolCanvas* canvas = ecs_field(it, SokolCanvas, 0);
   
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t e = it->entities[i];
        const char* name = ecs_get_name(it->world, e);
        SokolBuffer buffer = {
            .instance_count = 1,
            .index_count = 6,
            .colors = NULL,
            .transforms = NULL,
            .vertex_buffer = {0},
            .index_buffer = {0},
            .color_buffer = {0},
            .transform_buffer = {0}
        };

        vec3 vertices[] = {
            {-0.5f, -0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            { 0.5f,  0.5f, 0.0f},
            {-0.5f,  0.5f, 0.0f}
        };

        uint16_t indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        buffer.vertex_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(vertices),
                .data = SG_RANGE(vertices),
                .usage = SG_USAGE_IMMUTABLE
        });

        buffer.index_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(indices),
                .data = SG_RANGE(indices),
                .type = SG_BUFFERTYPE_INDEXBUFFER,
                .usage = SG_USAGE_IMMUTABLE
        });

        buffer.index_count = 6;
        buffer.instance_count = 2;

        buffer.colors = malloc(sizeof(Color) * buffer.instance_count);
        buffer.transforms = malloc(sizeof(Transform) * buffer.instance_count);

        float angle = 0.0f;

        for (int j = 0; j < buffer.instance_count; j++) {
            buffer.colors[j] = (Color){ 1.0f, 1.0f, 1.0f, 1.0f };
            glm_mat4_identity((mat4*)buffer.transforms[j].m);
            glm_rotate_z((mat4*)buffer.transforms[j].m, glm_rad(angle), (mat4*)buffer.transforms[j].m);
            angle += 30.0f;
        }

        buffer.color_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(Color) * buffer.instance_count,
                .usage = SG_USAGE_DYNAMIC
        });

        sg_update_buffer(buffer.color_buffer, &(sg_range) {
            .ptr = buffer.colors,
                .size = sizeof(Color) * buffer.instance_count
        });

        buffer.transform_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(Transform) * buffer.instance_count,
                .usage = SG_USAGE_DYNAMIC
        });

        sg_update_buffer(buffer.transform_buffer, &(sg_range) {
            .ptr = buffer.transforms,
                .size = sizeof(Transform) * buffer.instance_count
        });

        ecs_set_id(it->world, e, ecs_id(SokolBuffer), sizeof(SokolBuffer), &buffer);
        ecs_modified(it->world, e, SokolBuffer);

        canvas[i].pass_action = init_pass_action((Color) { 0.0f, 0.0f, 0.0f, 1.0f });
        canvas[i].pipeline = init_pipeline();
    }
}

void RenderTriangle(ecs_iter_t* it) {
    SokolCanvas* canvas = ecs_field(it, SokolCanvas, 0);
    SokolBuffer* buffer = ecs_field(it, SokolBuffer, 1);

    sg_begin_pass(&(sg_pass) {
        .action = canvas->pass_action,
            .swapchain = (sg_swapchain){ .width = state.width, .height = state.height }
    });

    for (int i = 0; i < it->count; i++) {
        sg_apply_pipeline(canvas[i].pipeline);

        sg_bindings bind = {
            .vertex_buffers[0] = buffer[i].vertex_buffer,
            .vertex_buffers[1] = buffer[i].color_buffer,
            .vertex_buffers[2] = buffer[i].transform_buffer,
            .index_buffer = buffer[i].index_buffer,
        };
        sg_apply_bindings(&bind);

        sg_draw(0, buffer[i].index_count, buffer[i].instance_count);
    }

    sg_end_pass();
    sg_commit();
}

void _sg_initialize() {
    sg_setup(&(sg_desc) { 0 });
    ecs_log_set_level(-1);
    world = ecs_init();


    ECS_COMPONENT_DEFINE(world, SokolCanvas);
    ECS_COMPONENT_DEFINE(world, SokolBuffer);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .callback = SokolSetCanvas,
            .events = { EcsOnSet },
            .query.terms = {
                {.id = ecs_id(SokolCanvas) },
                {.id = ecs_id(SokolBuffer), .oper = EcsNot },
                { 0 }
        }
    });

    ECS_SYSTEM(world, RenderTriangle, EcsOnUpdate, [in] SokolCanvas, [in] SokolBuffer);

    ecs_entity_t canvas_entity = ecs_new(world);

    ecs_set_name(world, canvas_entity, "CanvasEntity");

    ecs_add(world, canvas_entity, SokolCanvas);
    ecs_modified(world, canvas_entity, SokolCanvas);
}

void _sg_shutdown() {
    sg_shutdown();
    ecs_fini(world);
}

void _sg_render(int w, int h) {
    state.width = w;
    state.height = h;

    if (!ecs_progress(world, 0)) {
        printf("ecs_progress 运行时出错\n");
    }
}

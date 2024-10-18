
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"

static struct {
    
    int width;
    int height;
} state;


static ecs_world_t* world;


//typedef struct {
//    float vertices[9]; // 三个顶点，每个顶点有 x, y, z
//} Triangle;




typedef struct {
    sg_pass_action pass_action;
    sg_pipeline pipeline;
} SokolCanvas;


typedef struct {
    float m[16]; // 4x4 变换矩阵
} Transform;

typedef struct {
    float r, g, b, a;
} Color;

typedef struct SokolBuffer {
    /* GPU buffers */
    sg_buffer vertex_buffer;        /* Geometry (static) */
    sg_buffer index_buffer;         /* Indices (static) */
    sg_buffer color_buffer;         /* Color (per instance) */
    sg_buffer transform_buffer;     /* Transform (per instance) */

    /* Application-cached buffers */
    Color* colors;
    Transform* transforms;

    /* Number of instances */
    int32_t instance_count;

    /* Number of indices */
    int32_t index_count;
} SokolBuffer;




// 声明组件
//ECS_COMPONENT_DECLARE(Triangle);
ECS_COMPONENT_DECLARE(RenderData);
ECS_COMPONENT_DECLARE(SokolBuffer);



static
sg_pass_action init_pass_action(
    const Color bg_color)
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
    /* create a shader (use vertex attribute locations) */
    sg_shader shd = sg_make_shader(&(sg_shader_desc) {
        .vs.source =
            "#version 330\n"
            "layout(location=0) in vec4 position;\n"
            "layout(location=1) in vec4 color0;\n"
            "layout(location=2) in mat4 mat;\n"
            "out vec4 color;\n"
            "void main() {\n"
            "  gl_Position = mat * position;\n"
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

    /* create a pipeline object (default render state is fine) */
    return sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = shd,
            .index_type = SG_INDEXTYPE_UINT16,
            .layout = {
                .buffers = {
                    [1] = {.stride = 16, .step_func = SG_VERTEXSTEP_PER_INSTANCE },
                    [2] = {.stride = 64, .step_func = SG_VERTEXSTEP_PER_INSTANCE }
                },

                .attrs = {
                    /* Static geometry */
                    [0] = {.offset = 0,  .format = SG_VERTEXFORMAT_FLOAT3 },

                    /* Color buffer (per instance) */
                    [1] = {.buffer_index = 1,  .offset = 0, .format = SG_VERTEXFORMAT_FLOAT4 },

                    /* Matrix (per instance) */
                    [2] = {.buffer_index = 2,  .offset = 0,  .format = SG_VERTEXFORMAT_FLOAT4 },
                    [3] = {.buffer_index = 2,  .offset = 16, .format = SG_VERTEXFORMAT_FLOAT4 },
                    [4] = {.buffer_index = 2,  .offset = 32, .format = SG_VERTEXFORMAT_FLOAT4 },
                    [5] = {.buffer_index = 2,  .offset = 48, .format = SG_VERTEXFORMAT_FLOAT4 }
                }
        }
    });
}





static
void init_buffers(
    ecs_world_t* world)
{
    ecs_entity_t rect_buf = ecs_lookup_fullpath(
        world, "flecs.systems.sokol.RectangleBuffer");
    ecs_assert(rect_buf != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t sokol_buffer = ecs_lookup_fullpath(
        world, "flecs.systems.sokol.Buffer");
    ecs_assert(sokol_buffer != 0, ECS_INTERNAL_ERROR, NULL);

    SokolBuffer* b = ecs_get_mut_w_entity(world, rect_buf, sokol_buffer, NULL);
    ecs_assert(b != NULL, ECS_INTERNAL_ERROR, NULL);

    vec3 vertices[] = {
        {-0.5, -0.5, 0.0},
        { 0.5, -0.5, 0.0},
        { 0.5,  0.5, 0.0},
        {-0.5,  0.5, 0.0}
    };

    uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    b->vertex_buffer = sg_make_buffer(&(sg_buffer_desc) {
        .size = sizeof(vertices),
            .content = vertices,
            .usage = SG_USAGE_IMMUTABLE
    });

    b->index_buffer = sg_make_buffer(&(sg_buffer_desc) {
        .size = sizeof(indices),
            .content = indices,
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .usage = SG_USAGE_IMMUTABLE
    });

    b->index_count = 6;
}

static
void SokolSetCanvas(ecs_iter_t* it) {


    for (int i = 0; i < it->count; i++) {
        // 使用 ecs_get 获取 Triangle 组件
        SokolBuffer* b = ecs_get(it->world, it->entities[i], SokolBuffer);

        if (!b) {
 
            continue;
        }

        vec3 vertices[] = {
       {-0.5, -0.5, 0.0},
       { 0.5, -0.5, 0.0},
       { 0.5,  0.5, 0.0},
       {-0.5,  0.5, 0.0}
        };

        uint16_t indices[] = {
            0, 1, 2,
            0, 2, 3
        };

        b->vertex_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(vertices),
                .data = SG_RANGE(vertices),
                .usage = SG_USAGE_IMMUTABLE
        });

        b->index_buffer = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(indices),
                .data = SG_RANGE(indices),
                .type = SG_BUFFERTYPE_INDEXBUFFER,
                .usage = SG_USAGE_IMMUTABLE
        });

        b->index_count = 6;



    }



}


// InitRenderData 观察者
void InitRenderData(const ecs_iter_t* it) {
    for (int i = 0; i < it->count; i++) {
        // 使用 ecs_get 获取 Triangle 组件
        Triangle* t = ecs_get(it->world, it->entities[i], Triangle);

        if (!t) {
            // 如果未能获取组件，继续下一次循环
            continue;
        }

        // 创建顶点缓冲区
        sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(t->vertices),
                .data = SG_RANGE(t->vertices),
                .label = "triangle-vertices"
        });

        // 创建着色器
        sg_shader shd = sg_make_shader(&(sg_shader_desc) {
            .attrs = {
                [0] .name = "position"
            },
                .vs.source =
                "#version 330\n"
                "layout(location = 0) in vec4 position;\n"
                "void main() {\n"
                "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
                "}\n",
                .fs.source =
                "#version 330\n"
                "layout(location = 0) out vec4 FragColor;\n"
                "void main() {\n"
                "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                "}\n"
        });

        // 创建渲染管线
        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc) {
            .shader = shd,
                .layout = {
                    .attrs = {
                        [0] .format = SG_VERTEXFORMAT_FLOAT3
                    }
            },
                .label = "triangle-pipeline"
        });

        // 添加 RenderData 组件到实体
        ecs_set(it->world, it->entities[i], RenderData, {
            .vertex_buffer = vbuf,
            .pipeline = pip
            });
    }
}

// RenderTriangle 系统
void RenderTriangle(const ecs_iter_t* it) {

    printf("RenderTriangle called: field_count = %d\n", it->field_count);

    // 检查 field_count 是否至少为 1
    if (it->field_count < 1) {
        printf("No fields available in iterator\n");
        return;
    }


    RenderData* rd = ecs_field(it, RenderData, 0);

    // 开始渲染通道

    sg_begin_pass(&(sg_pass) {
        .action = state.pass_action,
            .swapchain = (sg_swapchain){ .width = state.width, .height = state.height }
    });


    for (int i = 0; i < it->count; i++) {
        // 应用渲染管线
        sg_apply_pipeline(rd[i].pipeline);

        // 设置绑定
        sg_bindings bind = {
            .vertex_buffers[0] = rd[i].vertex_buffer
        };
        sg_apply_bindings(&bind);

        // 绘制
        sg_draw(0, 3, 1);
    }

    // 结束渲染通道
    sg_end_pass();
    sg_commit();
}




void _sg_initialize() {
    // 初始化 sokol_gfx
    sg_setup(&(sg_desc) { 0 });

    // 初始化渲染操作

    state.pass_action = (sg_pass_action){
         .colors[0] = {.load_action = SG_LOADACTION_CLEAR,  .clear_value = {0.6f, 0.6f, 0.9f, 1.0f} }
    };


    ecs_log_set_level(0); // 设置日志级别为调试
    // 初始化 Flecs 世界
    world = ecs_init();

    // 注册组件
    ECS_COMPONENT_DEFINE(world, Triangle);
    ECS_COMPONENT_DEFINE(world, RenderData);



    // 注册观察者
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .callback = InitRenderData,
            .events = { EcsOnSet },
            .query = {
                .terms = {
                    {.id = ecs_id(Triangle) },
                    {.id = ecs_id(RenderData), .oper = EcsNot },
                    {.id = 0 } // 终止 terms 数组
                }
        }
    });

    ECS_SYSTEM(world, RenderTriangle, EcsOnUpdate, [in] RenderData);



    // 创建实体并附加 Triangle 组件
    ecs_entity_t triangle_entity = ecs_new(world);



    ecs_set(world, triangle_entity, Triangle, {
        .vertices = {
            -0.5f, -0.5f, 0.0f,     // 左下角
            0.5f, -0.5f, 0.0f,      // 右下角
            0.0f,  0.5f, 0.0f       // 顶部
        }
        });
}



void _sg_shutdown() {
    // 清理 sokol_gfx
    sg_shutdown();

    // 清理 Flecs 世界
    ecs_fini(world);
}

void _sg_render(int w, int h) {
    // 更新窗口尺寸
    state.width = w;
    state.height = h;

    // 运行 ECS 世界
    if (!ecs_progress(world, 0)) {
        printf("An error occurred during ecs_progress\n");
    }
}

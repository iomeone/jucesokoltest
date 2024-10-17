
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"

static struct {
    sg_pass_action pass_action;
    int width;
    int height;
} state;


static ecs_world_t* world;


typedef struct {
    float vertices[9]; // 三个顶点，每个顶点有 x, y, z
} Triangle;




typedef struct {
    sg_buffer vertex_buffer;
    sg_pipeline pipeline;
} RenderData;




// 声明组件
ECS_COMPONENT_DECLARE(Triangle);
ECS_COMPONENT_DECLARE(RenderData);



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

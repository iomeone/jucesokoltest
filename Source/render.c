
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
    float vertices[9]; // �������㣬ÿ�������� x, y, z
} Triangle;




typedef struct {
    sg_buffer vertex_buffer;
    sg_pipeline pipeline;
} RenderData;




// �������
ECS_COMPONENT_DECLARE(Triangle);
ECS_COMPONENT_DECLARE(RenderData);



// InitRenderData �۲���
void InitRenderData(const ecs_iter_t* it) {
    for (int i = 0; i < it->count; i++) {
        // ʹ�� ecs_get ��ȡ Triangle ���
        Triangle* t = ecs_get(it->world, it->entities[i], Triangle);

        if (!t) {
            // ���δ�ܻ�ȡ�����������һ��ѭ��
            continue;
        }

        // �������㻺����
        sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc) {
            .size = sizeof(t->vertices),
                .data = SG_RANGE(t->vertices),
                .label = "triangle-vertices"
        });

        // ������ɫ��
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

        // ������Ⱦ����
        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc) {
            .shader = shd,
                .layout = {
                    .attrs = {
                        [0] .format = SG_VERTEXFORMAT_FLOAT3
                    }
            },
                .label = "triangle-pipeline"
        });

        // ��� RenderData �����ʵ��
        ecs_set(it->world, it->entities[i], RenderData, {
            .vertex_buffer = vbuf,
            .pipeline = pip
            });
    }
}

// RenderTriangle ϵͳ
void RenderTriangle(const ecs_iter_t* it) {

    printf("RenderTriangle called: field_count = %d\n", it->field_count);

    // ��� field_count �Ƿ�����Ϊ 1
    if (it->field_count < 1) {
        printf("No fields available in iterator\n");
        return;
    }


    RenderData* rd = ecs_field(it, RenderData, 0);

    // ��ʼ��Ⱦͨ��

    sg_begin_pass(&(sg_pass) {
        .action = state.pass_action,
            .swapchain = (sg_swapchain){ .width = state.width, .height = state.height }
    });


    for (int i = 0; i < it->count; i++) {
        // Ӧ����Ⱦ����
        sg_apply_pipeline(rd[i].pipeline);

        // ���ð�
        sg_bindings bind = {
            .vertex_buffers[0] = rd[i].vertex_buffer
        };
        sg_apply_bindings(&bind);

        // ����
        sg_draw(0, 3, 1);
    }

    // ������Ⱦͨ��
    sg_end_pass();
    sg_commit();
}




void _sg_initialize() {
    // ��ʼ�� sokol_gfx
    sg_setup(&(sg_desc) { 0 });

    // ��ʼ����Ⱦ����

    state.pass_action = (sg_pass_action){
         .colors[0] = {.load_action = SG_LOADACTION_CLEAR,  .clear_value = {0.6f, 0.6f, 0.9f, 1.0f} }
    };


    ecs_log_set_level(0); // ������־����Ϊ����
    // ��ʼ�� Flecs ����
    world = ecs_init();

    // ע�����
    ECS_COMPONENT_DEFINE(world, Triangle);
    ECS_COMPONENT_DEFINE(world, RenderData);



    // ע��۲���
    ecs_observer_init(world, &(ecs_observer_desc_t){
        .callback = InitRenderData,
            .events = { EcsOnSet },
            .query = {
                .terms = {
                    {.id = ecs_id(Triangle) },
                    {.id = ecs_id(RenderData), .oper = EcsNot },
                    {.id = 0 } // ��ֹ terms ����
                }
        }
    });

    ECS_SYSTEM(world, RenderTriangle, EcsOnUpdate, [in] RenderData);



    // ����ʵ�岢���� Triangle ���
    ecs_entity_t triangle_entity = ecs_new(world);



    ecs_set(world, triangle_entity, Triangle, {
        .vertices = {
            -0.5f, -0.5f, 0.0f,     // ���½�
            0.5f, -0.5f, 0.0f,      // ���½�
            0.0f,  0.5f, 0.0f       // ����
        }
        });
}



void _sg_shutdown() {
    // ���� sokol_gfx
    sg_shutdown();

    // ���� Flecs ����
    ecs_fini(world);
}

void _sg_render(int w, int h) {
    // ���´��ڳߴ�
    state.width = w;
    state.height = h;

    // ���� ECS ����
    if (!ecs_progress(world, 0)) {
        printf("An error occurred during ecs_progress\n");
    }
}

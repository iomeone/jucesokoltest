
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"
#include <cglm.h>
 


struct EcsRgb {
    float r;
    float g;
    float b;
};
typedef EcsRgb ecs_rgb_t ;
typedef EcsRgb ecs_rgba_t;


struct EcsPosition3
{
    float x;
    float y;
    float z;
};


struct EcsRectangle {
    float width;
    float height;
};


 struct EcsTransform3 {
    mat4 value;
} ;










struct SokolCanvas {
    sg_pass_action pass_action;
    sg_pipeline pip;

    EcsRgb  background_color;
};

typedef SokolCanvas EcsCanvas;



struct SokolBuffer {
    // GPU buffers
    sg_buffer vertex_buffer;        // Geometry (static)
    sg_buffer index_buffer;         // Indices (static)
    sg_buffer color_buffer;         // Color (per instance)
    sg_buffer transform_buffer;     // Transform (per instance)

    // Application-cached buffers
    EcsRgb* colors;
    mat4* transforms;

    // Number of instances
    int32_t instance_count;

    // Number of indices
    int32_t index_count;

    // Constructor
    SokolBuffer()
        : colors(nullptr), transforms(nullptr), instance_count(0), index_count(0) {
        vertex_buffer = { 0 };
        index_buffer = { 0 };
        color_buffer = { 0 };
        transform_buffer = { 0 };
    }

    ~SokolBuffer() {
        ecs_os_free(colors);
        ecs_os_free(transforms);
    }
};






static
sg_pass_action init_pass_action(const EcsCanvas* canvas) {
    ecs_rgb_t bg_color = canvas->background_color;

    sg_pass_action pass_action = {};
    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    pass_action.colors[0].clear_value.r = bg_color.r;
    pass_action.colors[0].clear_value.g = bg_color.g;
    pass_action.colors[0].clear_value.b = bg_color.b;
    pass_action.colors[0].clear_value.a = 1.0f;

    return pass_action;


}

 

static
sg_pipeline init_pipeline() {
    sg_shader_desc shader_desc = {};
    shader_desc.vs.source =
        "#version 330\n"
        "layout(location=0) in vec4 position;\n"
        "layout(location=1) in vec4 color0;\n"
        "layout(location=2) in mat4 mat;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "  gl_Position = mat * position;\n"
        "  color = color0;\n"
        "}\n";
    shader_desc.fs.source =
        "#version 330\n"
        "in vec4 color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "  frag_color = color;\n"
        "}\n";

    sg_shader shd = sg_make_shader(&shader_desc);

    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.shader = shd;
    pipeline_desc.index_type = SG_INDEXTYPE_UINT16;

    // 设置布局
    pipeline_desc.layout.buffers[1].stride = 16;
    pipeline_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE;
    pipeline_desc.layout.buffers[2].stride = 64;
    pipeline_desc.layout.buffers[2].step_func = SG_VERTEXSTEP_PER_INSTANCE;

    // 设置属性
    // Static geometry
    pipeline_desc.layout.attrs[0].offset = 0;
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;

    // Color buffer (per instance)
    pipeline_desc.layout.attrs[1].buffer_index = 1;
    pipeline_desc.layout.attrs[1].offset = 0;
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4;

    // Matrix (per instance)
    pipeline_desc.layout.attrs[2].buffer_index = 2;
    pipeline_desc.layout.attrs[2].offset = 0;
    pipeline_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[3].buffer_index = 2;
    pipeline_desc.layout.attrs[3].offset = 16;
    pipeline_desc.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[4].buffer_index = 2;
    pipeline_desc.layout.attrs[4].offset = 32;
    pipeline_desc.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4;

    pipeline_desc.layout.attrs[5].buffer_index = 2;
    pipeline_desc.layout.attrs[5].offset = 48;
    pipeline_desc.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT4;

    return sg_make_pipeline(&pipeline_desc);
}





static
void init_buffers(flecs::world& ecs) {
    auto rect_buf = ecs.lookup("flecs.systems.sokol.RectangleBuffer");
    ecs_assert(rect_buf.is_alive(), ECS_INTERNAL_ERROR, NULL);

    auto b = rect_buf.get_mut<SokolBuffer>();
    ecs_assert(b != nullptr, ECS_INTERNAL_ERROR, NULL);

    mat3 vertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f}
    };

    uint16_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.size = sizeof(vertices);
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.usage = SG_USAGE_IMMUTABLE;

    b->vertex_buffer = sg_make_buffer(&vbuf_desc);

    sg_buffer_desc ibuf_desc = {};
    ibuf_desc.size = sizeof(indices);
    ibuf_desc.data = SG_RANGE(indices);
    ibuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    ibuf_desc.usage = SG_USAGE_IMMUTABLE;

    b->index_buffer = sg_make_buffer(&ibuf_desc);

    b->index_count = 6;
}


flecs::world world;

void _sg_initialize() 
{
    ecs_log_set_level(1);




    world.component<SokolCanvas>();
    world.component<SokolBuffer>();





    world.system<const EcsCanvas>()
        .kind(flecs::OnSet)
        .each([&](flecs::entity e, const EcsCanvas& canvas) {

        sg_desc desc = {};
        sg_setup(&desc);
        assert(sg_isvalid());

        SokolCanvas sokol_canvas;

        sokol_canvas.pass_action = init_pass_action(&canvas);
        sokol_canvas.pip = init_pipeline();

        e.set<SokolCanvas>(sokol_canvas);

        init_buffers(world);

        ecs_trace("sokol initialized");
    });



    auto SokolRectangleBuffer = world.entity("SokolRectangleBuffer")
        .add<SokolBuffer>();




    auto rectangle_query = world.query_builder<const EcsPosition3, const EcsRectangle, const EcsRgb, const EcsTransform3>()
        .build();



    // 系统来管理矩形的缓冲区
    world.system<SokolBuffer>()
        .kind(flecs::PostLoad)
        .each([&](flecs::entity e, SokolBuffer& b) {
        // 检查查询是否发生变化
        if (!rectangle_query.changed()) {
            return;
        }

        int32_t count = 0;
        rectangle_query.each([&](flecs::entity, const EcsPosition3&, const EcsRectangle&, const EcsRgb&, const EcsTransform3&) {
            count++;
            });

        if (count == 0) {
            if (b.instance_count == 0) {
                // 无需处理
                return;
            }
        }

        // 分配或重新分配缓冲区
        size_t colors_size = count * sizeof(ecs_rgba_t);
        size_t transforms_size = count * sizeof(mat4);

        if (b.instance_count < count) {
            b.colors = (ecs_rgba_t*)ecs_os_realloc(b.colors, colors_size);
            b.transforms = (mat4*)ecs_os_realloc(b.transforms, transforms_size);
        }

        int32_t cursor = 0;

        rectangle_query.each([&](flecs::entity, const EcsPosition3&, const EcsRectangle& rect, const EcsRgb& color,  const EcsTransform3& transform) {
            // 复制颜色
            b.colors[cursor] = color;

            // 复制变换矩阵
            glm_mat4_copy(const_cast<mat4&>(transform.value), b.transforms[cursor]);


            // 应用缩放变换
            vec3 scale = { rect.width, rect.height, 1.0f };
            glm_scale(b.transforms[cursor], scale);

            cursor++;
            });

        b.instance_count = count;

        // 创建或更新缓冲区
        if (b.color_buffer.id == SG_INVALID_ID) {
            sg_buffer_desc color_buf_desc = {};
            color_buf_desc.size = colors_size;
            color_buf_desc.usage = SG_USAGE_STREAM;

            b.color_buffer = sg_make_buffer(&color_buf_desc);
        }

        if (b.transform_buffer.id == SG_INVALID_ID) {
            sg_buffer_desc transform_buf_desc = {};
            transform_buf_desc.size = transforms_size;
            transform_buf_desc.usage = SG_USAGE_STREAM;

            b.transform_buffer = sg_make_buffer(&transform_buf_desc);
        }

        sg_update_buffer(b.color_buffer, { .ptr = b.colors, .size = colors_size });
        sg_update_buffer(b.transform_buffer, { .ptr = b.transforms, .size = transforms_size });

           
      });





    world.system<const SokolCanvas>()
        .kind(flecs::OnStore)
        .each([&](flecs::entity e, const SokolCanvas& canvas) {
        int w, h;


        SDL_GL_GetDrawableSize(canvas.sdl_window, &w, &h);

        sg_begin_default_pass(&canvas.pass_action, w, h);
        sg_apply_pipeline(canvas.pip);

        world.each([&](flecs::entity e, SokolBuffer& buffer) {
            if (buffer.instance_count == 0) {
                return;
            }

            sg_bindings bind = {};
            bind.vertex_buffers[0] = buffer.vertex_buffer;
            bind.vertex_buffers[1] = buffer.color_buffer;
            bind.vertex_buffers[2] = buffer.transform_buffer;
            bind.index_buffer = buffer.index_buffer;

            sg_apply_bindings(&bind);
            sg_draw(0, buffer.index_count, buffer.instance_count);
            });

        sg_end_pass();
        sg_commit();
 
            });






}



void _sg_shutdown() 
{

}




void _sg_render(int w, int h)
{
 
    world.progress();

}

// 
//void InitRenderData(const ecs_iter_t* it) {
//    for (int i = 0; i < it->count; i++) {
// 
//        const Triangle* t = ecs_get(it->world, it->entities[i], Triangle);
//
//        if (!t) {
//  
//            continue;
//        }
//
// 
//    
//        sg_buffer_desc vbuf_desc = {};
//        vbuf_desc.size = sizeof(t->vertices);
//        vbuf_desc.data = SG_RANGE(t->vertices);
//        vbuf_desc.label = "triangle-vertices";
//
//        sg_buffer vbuf = sg_make_buffer(&vbuf_desc);
//       
//
//
//        // ´´½¨×ÅÉ«Æ÷
//        sg_shader_desc shd_desc = {};
//        shd_desc.attrs[0].name = "position";
//        shd_desc.vs.source =
//            "#version 330\n"
//            "layout(location = 0) in vec4 position;\n"
//            "void main() {\n"
//            "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
//            "}\n";
//        shd_desc.fs.source =
//            "#version 330\n"
//            "layout(location = 0) out vec4 FragColor;\n"
//            "void main() {\n"
//            "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
//            "}\n";
//
//        sg_shader shd = sg_make_shader(&shd_desc);
//
//        // 创建 Pipeline
//        sg_pipeline_desc pip_desc = {};
//        pip_desc.shader = shd;
//        pip_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3;
//        pip_desc.label = "triangle-pipeline";
//
//        sg_pipeline pip = sg_make_pipeline(&pip_desc);
//
//        // 添加 RenderData 组件到实体
//        RenderData render_data = {};
//        render_data.vertex_buffer = vbuf;
//        render_data.pipeline = pip;
//
//    
//        //ecs_set(it->world, it->entities[i], RenderData, render_data);
//
//    }
//}
//
//// RenderTriangle ÏµÍ³
//void RenderTriangle(const ecs_iter_t* it) {
//
//    printf("RenderTriangle called: field_count = %d\n", it->field_count);
//
//    // ¼ì²é field_count ÊÇ·ñÖÁÉÙÎª 1
//    if (it->field_count < 1) {
//        printf("No fields available in iterator\n");
//        return;
//    }
//
//
//    RenderData* rd = ecs_field(it, RenderData, 0);
//
//    // ¿ªÊ¼äÖÈ¾Í¨µÀ
//
//// 开始渲染 pass
//    sg_pass pass = {};
//    pass.action = state.pass_action;
//    pass.swapchain.width = state.width;
//    pass.swapchain.height = state.height;
//    sg_begin_pass(&pass);
//
//    // 渲染循环
//    for (int i = 0; i < it->count; i++) {
//        // 应用渲染管线
//        sg_apply_pipeline(rd[i].pipeline);
//
//        // 设置绑定
//        sg_bindings bind = {};
//        bind.vertex_buffers[0] = rd[i].vertex_buffer;
//        sg_apply_bindings(&bind);
//
//        // 渲染
//        sg_draw(0, 3, 1);
//    }
//
//  
//    sg_end_pass();
//    sg_commit();
//}
//
//
//#include <iostream>
//
//
// 
//struct Position {
//    float x, y;
//};
//
//struct Velocity {
//    float x, y;
//};
//
//
//void init()
//{
//
//    flecs::world ecs;
//
//    // 注册组件
//    ecs.component<Position>();
//    ecs.component<Velocity>();
//
//    // 创建一些实体，初始化 Position 和 Velocity 组件
//    auto e1 = ecs.entity()
//        .set<Position>({ 0, 0 })
//        .set<Velocity>({ 1, 1 });
//
//    auto e2 = ecs.entity()
//        .set<Position>({ 5, 5 })
//        .set<Velocity>({ 2, 2 });
//
//    // 创建查询，寻找具有 Position 和 Velocity 组件的实体
//    auto query = ecs.query_builder<Position, Velocity>()
//        .cached()
//        .build();
//
//    // 第一次查询结果
//    std::cout << "Initial query:" << std::endl;
//    query.each([](flecs::entity e, Position& p, Velocity& v) {
//        std::cout << "Entity " << e.id() << ": Position(" << p.x << ", " << p.y << "), Velocity(" << v.x << ", " << v.y << ")" << std::endl;
//        });
//
//    if (query.changed()) {
//        std::cout << "\nQuery changed ??" << std::endl;
//
//    }
//
//    query.each([](flecs::entity e, Position& p, Velocity& v) {
//        std::cout << "Entity " << e.id() << ": Position(" << p.x << ", " << p.y << "), Velocity(" << v.x << ", " << v.y << ")" << std::endl;
//        });
//
//    if (query.changed()) {
//        std::cout << "\nQuery changed ??" << std::endl;
//
//    }
//    // 添加新的实体，触发查询变化
//    auto e3 = ecs.entity()
//        .set<Position>({ 10, 10 })
//        .set<Velocity>({ 3, 3 });
//
//    // 检查查询是否有变化
//    if (query.changed()) {
//        std::cout << "\nQuery changed after adding a new entity:" << std::endl;
//        query.each([](flecs::entity e, Position& p, Velocity& v) {
//            std::cout << "Entity " << e.id() << ": Position(" << p.x << ", " << p.y << "), Velocity(" << v.x << ", " << v.y << ")" << std::endl;
//            });
//    }
//
//    // 修改现有实体的 Position，触发查询变化
//    e1.set<Position>({ 20, 20 });
//
//    // 再次检查查询是否有变化
//    if (query.changed()) {
//        std::cout << "\nQuery changed after updating an entity's Position:" << std::endl;
//        query.each([](flecs::entity e, Position& p, Velocity& v) {
//            std::cout << "Entity " << e.id() << ": Position(" << p.x << ", " << p.y << "), Velocity(" << v.x << ", " << v.y << ")" << std::endl;
//            });
//    }
//}
//
//
//
//void _sg_initialize() {
//    
//    init();
//
//    // 初始化 Sokol
//    sg_desc desc = {};
//    sg_setup(&desc);
//
//    // 设置 pass_action
//    sg_pass_action pass_action = {};
//    pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
//    pass_action.colors[0].clear_value.r = 0.6f;  // 假设 .r 是红色通道
//    pass_action.colors[0].clear_value.g = 0.6f;  // 假设 .g 是绿色通道
//    pass_action.colors[0].clear_value.b = 0.9f;  // 假设 .b 是蓝色通道
//    pass_action.colors[0].clear_value.a = 1.0f;  // 假设 .a 是 alpha 通道
//
//
//    state.pass_action = pass_action;
//
//    ecs_log_set_level(-1); // ÉèÖÃÈÕÖ¾¼¶±ðÎªµ÷ÊÔ
//    // ³õÊ¼»¯ Flecs ÊÀ½ç
//    world = ecs_init();
//
//   
//    ECS_COMPONENT_DEFINE(world, Triangle);
//    ECS_COMPONENT_DEFINE(world, RenderData);
//
//
//
// 
//    //ecs_observer_init(world, &(ecs_observer_desc_t){
//    //    .callback = InitRenderData,
//    //        .events = { EcsOnSet },
//    //        .query = {
//    //            .terms = {
//    //                {.id = ecs_id(Triangle) },
//    //                {.id = ecs_id(RenderData), .oper = EcsNot },
//    //                {.id = 0 } // ÖÕÖ¹ terms Êý×é
//    //            }
//    //    }
//    //});
//
//    //ECS_SYSTEM(world, RenderTriangle, EcsOnUpdate, [in] RenderData);
//
//
//
//    //// ´´½¨ÊµÌå²¢¸½¼Ó Triangle ×é¼þ
//    //ecs_entity_t triangle_entity = ecs_new(world);
//
//
//
//    //ecs_set(world, triangle_entity, Triangle, {
//    //    .vertices = {
//    //        -0.5f, -0.5f, 0.0f,     // ×óÏÂ½Ç
//    //        0.5f, -0.5f, 0.0f,      // ÓÒÏÂ½Ç
//    //        0.0f,  0.5f, 0.0f       // ¶¥²¿
//    //    }
//    //    });
//}
//
//
//
//void _sg_shutdown() {
//    // ÇåÀí sokol_gfx
//    sg_shutdown();
//
//    // ÇåÀí Flecs ÊÀ½ç
//    ecs_fini(world);
//}
//
//void _sg_render(int w, int h) {
//    // ¸üÐÂ´°¿Ú³ß´ç
//    state.width = w;
//    state.height = h;
//
//    // ÔËÐÐ ECS ÊÀ½ç
//    if (!ecs_progress(world, 0)) {
//        printf("An error occurred during ecs_progress\n");
//    }
//}

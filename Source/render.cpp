
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"

 
struct SokolCanvas {
    sg_pass_action pass_action;
    sg_pipeline pip;
};

struct EcsRgb {
    float r;
    float g;
    float b;
};

struct SokolBuffer {
    // GPU buffers
    sg_buffer vertex_buffer;        // Geometry (static)
    sg_buffer index_buffer;         // Indices (static)
    sg_buffer color_buffer;         // Color (per instance)
    sg_buffer transform_buffer;     // Transform (per instance)

    // Application-cached buffers
    EcsRgb* colors;
    glm::mat4* transforms;

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

//
//
//static struct {
//    sg_pass_action pass_action;
//    int width;
//    int height;
//} state;
//
//
//static ecs_world_t* world;
//
//
//typedef struct {
//    float vertices[9];  
//} Triangle;
//
//
//
//
//typedef struct {
//    sg_buffer vertex_buffer;
//    sg_pipeline pipeline;
//} RenderData;
//
//
//
//
// 
//ECS_COMPONENT_DECLARE(Triangle);
//ECS_COMPONENT_DECLARE(RenderData);
//
//
//
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

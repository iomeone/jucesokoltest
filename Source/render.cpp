
#include "render.h"


#include "sokol_gfx.h"

#include "flecs.h"
#include <cglm.h>
 


struct EcsRgb {
    float r;
    float g;
    float b;
    float a;
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

    // 配置缓冲区布局
    pipeline_desc.layout.buffers[0].stride = sizeof(float) * 3; // 顶点位置
    pipeline_desc.layout.buffers[0].step_func = SG_VERTEXSTEP_PER_VERTEX;

    pipeline_desc.layout.buffers[1].stride = sizeof(EcsRgb) ;     // 颜色
    pipeline_desc.layout.buffers[1].step_func = SG_VERTEXSTEP_PER_INSTANCE; // 每实例步进

    pipeline_desc.layout.buffers[2].stride = sizeof(EcsTransform3); // 变换矩阵
    pipeline_desc.layout.buffers[2].step_func = SG_VERTEXSTEP_PER_INSTANCE; // 每实例步进

    // 配置顶点属性
    pipeline_desc.layout.attrs[0].buffer_index = 0;
    pipeline_desc.layout.attrs[0].offset = 0;
    pipeline_desc.layout.attrs[0].format = SG_VERTEXFORMAT_FLOAT3; // 顶点位置属性 (vec3)

    pipeline_desc.layout.attrs[1].buffer_index = 1;
    pipeline_desc.layout.attrs[1].offset = 0;
    pipeline_desc.layout.attrs[1].format = SG_VERTEXFORMAT_FLOAT4; // 颜色属性 (vec4)

    pipeline_desc.layout.attrs[2].buffer_index = 2;
    pipeline_desc.layout.attrs[2].offset = 0;
    pipeline_desc.layout.attrs[2].format = SG_VERTEXFORMAT_FLOAT4; // 变换矩阵第一行 (vec4)

    pipeline_desc.layout.attrs[3].buffer_index = 2;
    pipeline_desc.layout.attrs[3].offset = 16;
    pipeline_desc.layout.attrs[3].format = SG_VERTEXFORMAT_FLOAT4; // 变换矩阵第二行 (vec4)

    pipeline_desc.layout.attrs[4].buffer_index = 2;
    pipeline_desc.layout.attrs[4].offset = 32;
    pipeline_desc.layout.attrs[4].format = SG_VERTEXFORMAT_FLOAT4; // 变换矩阵第三行 (vec4)

    pipeline_desc.layout.attrs[5].buffer_index = 2;
    pipeline_desc.layout.attrs[5].offset = 48;
    pipeline_desc.layout.attrs[5].format = SG_VERTEXFORMAT_FLOAT4; // 变换矩阵第四行 (vec4)

    // 创建渲染管道
    return sg_make_pipeline(&pipeline_desc);


  

}





static
void init_buffers(flecs::world& ecs) {
    auto rect_buf = ecs.lookup("SokolRectangleBuffer");
    ecs_assert(rect_buf.is_alive(), ECS_INTERNAL_ERROR, NULL);

    auto b = rect_buf.get_mut<SokolBuffer>();
    ecs_assert(b != nullptr, ECS_INTERNAL_ERROR, NULL);

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


//#include <sokol_log.h>  // Sokol的日志支持


// 自定义日志函数
void my_log(const char* tag, uint32_t log_level, uint32_t log_item_id,
    const char* message_or_null, uint32_t line_nr,
    const char* filename_or_null, void* user_data) {

    // 根据日志级别设置日志级别描述
    const char* log_level_str = "INFO";
    if (log_level == 0) log_level_str = "PANIC";
    else if (log_level == 1) log_level_str = "ERROR";
    else if (log_level == 2) log_level_str = "WARNING";

    // 打印日志信息到控制台
    printf("[%s] Level: %s, Item ID: %d, Message: %s, Line: %d, File: %s\n",
        tag,
        log_level_str,
        log_item_id,
        message_or_null ? message_or_null : "No message",
        line_nr,
        filename_or_null ? filename_or_null : "No file");
}


int global_width = 0;
int global_height = 0;


flecs::world world;
flecs::query<const EcsPosition3, const EcsRectangle, const EcsRgb, const EcsTransform3> rectangle_query;


void _sg_initialize(int w, int h) 
{
    global_width = w;
    global_height = h;
    ecs_log_set_level(1);



    world.component<EcsPosition3>();
    world.component<EcsRectangle>();
    world.component<EcsRgb>();
    world.component<EcsTransform3>();
    world.component<SokolCanvas>();
    world.component<SokolBuffer>();





    auto SokolRectangleBuffer = world.entity("SokolRectangleBuffer")
        .add<SokolBuffer>();



    sg_logger logger = {
          .func = my_log,  // 设置自定义日志函数
          .user_data = NULL  // 传递自定义用户数据（这里为 NULL）
    };


    // **直接在初始化函数中调用 sg_setup**
    sg_desc desc = {
     .logger = logger 
    };
    sg_setup(&desc);
    assert(sg_isvalid());  // 确保 Sokol 已经初始化

    // 初始化 SokolCanvas
    SokolCanvas sokol_canvas;
    // 设置背景颜色，您可以根据需要修改
    sokol_canvas.background_color = { 0.2f, 0.1f, 0.1f }; // 灰色背景
    sokol_canvas.pass_action = init_pass_action(&sokol_canvas);
    sokol_canvas.pip = init_pipeline();

    // 创建一个带有 EcsCanvas 组件的实体
    world.entity()
        .set<EcsCanvas>(sokol_canvas);

    // 初始化缓冲区
    init_buffers(world);








    rectangle_query = world.query_builder<const EcsPosition3, const EcsRectangle, const EcsRgb, const EcsTransform3>()
        .cached()
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
        .each([](flecs::entity e, const SokolCanvas& canvas) {
   

        sg_pass pass = {};
 

        sg_swapchain swapchain = {};
        swapchain.width = global_width;
        swapchain.height = global_width;


        pass.action = canvas.pass_action;
        pass.swapchain = swapchain;

        sg_begin_pass(&pass);


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





    auto init_transform = [](EcsTransform3& transform, const EcsPosition3& position) {
        glm_mat4_identity(transform.value); // 初始化为单位矩阵


        vec3 translation = { position.x, position.y, position.z };
        glm_translate(transform.value, translation);

        };



    // 创建第一个矩形实体
    EcsPosition3 pos1 = { 0.0f, 0.0f, 0.0f };
    EcsRectangle rect1 = { 1.0f, 1.0f };
    EcsRgb color1 = { .4f, 0.0f, 0.0f, 1.0f };
    EcsTransform3 transform1;
    init_transform(transform1, pos1);

    world.entity()
        .set<EcsPosition3>(pos1)
        .set<EcsRectangle>(rect1)
        .set<EcsRgb>(color1)
        .set<EcsTransform3>(transform1);




    // 创建第二个矩形实体
    EcsPosition3 pos2 = { .3f, 0.0f, 0.0f }; // 位于x轴正方向2.0的位置
    EcsRectangle rect2 = { 1.0f, 1.0f }; // 宽度和高度为1.0
    EcsRgb color2 = { 0.0f, .4f, 0.0f, 1.0f }; // 绿色
    EcsTransform3 transform2;
    init_transform(transform2, pos2);

    world.entity()
        .set<EcsPosition3>(pos2)
        .set<EcsRectangle>(rect2)
        .set<EcsRgb>(color2)
        .set<EcsTransform3>(transform2);



}



void _sg_shutdown() 
{

}




void _sg_render(int w, int h)
{
    global_width = w;
    global_height = h;
    world.progress();

}
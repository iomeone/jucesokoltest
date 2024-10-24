#include "render.h"
#include "flecs.h"
#include "flecs_systems_sokol.h"
#include "flecs_components_graphics.h"
#include "flecs_components_transform.h"
#include "flecs_components_geometry.h"

static ecs_world_t* world;

extern int app_width;

extern int app_height;

extern char* dir_of_etc_sokol_shaders ;






FILE* log_file = NULL;

void my_log(int level, const char* file, int32_t line, const char* msg) {
    if (log_file) {
        fprintf(log_file, "[%s:%d] %s\n", file, line, msg);
        fflush(log_file);
    }
}

void my_err(const char* fmt, va_list args) {
    if (log_file) {
        vfprintf(log_file, fmt, args);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}

void setup_custom_logging(const char* filename) {
    log_file = fopen(filename, "w");
    if (!log_file) {
        fprintf(stderr, "无法打开日志文件: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    // 获取默认的操作系统接口
    ecs_os_set_api_defaults();

    // 创建自定义的操作系统接口
    ecs_os_api_t os_api = ecs_os_api;

    // 设置自定义的日志函数
    os_api.log_ = my_log;


    // 应用自定义的操作系统接口
    ecs_os_set_api(&os_api);
}


 
void _sg_initialize(int w, int h) {

    app_width = w;
    app_height = h;

    dir_of_etc_sokol_shaders = "D:/etc/jucesokoltest/Source/";


    setup_custom_logging("flecs_log.txt");
    world = ecs_init();
    ecs_log_set_level(-2);


    // 导入必要的模块

    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsGeometry);
    ECS_IMPORT(world, FlecsSystemsSokol);
    //ECS_IMPORT(world, FlecsComponentsGraphics);
   


    // 创建摄像机实体并设置 EcsCamera 组件
    ecs_entity_t camera = ecs_new(world);
    ecs_set(world, camera, EcsCamera, {
        .position = {0.0f, 0.0f, -5.0f},  // 摄像机位置
        .lookat = {0.0f, 0.0f, 5.0f},     // 摄像机注视点
        .up = {0.0f, 1.0f, 0.0f},         // 摄像机上方向
        .fov = 60.0f,                     // 视野角度（以度为单位）
        .near_ = 0.1f,                    // 近裁剪面
        .far_ = 1000.0f,                  // 远裁剪面
        .ortho = true                    // 是否使用正交投影（false 表示透视投影）
        });

    // 创建画布实体并设置 EcsCanvas 组件
    ecs_entity_t canvas = ecs_new(world);
    ecs_set(world, canvas, EcsCanvas, {
        .title = "Hello!",
        .width = 800,
        .height = 600,
        .camera = camera,                // 将摄像机实体赋值给画布的 camera 字段
        .background_color = {0.0f, 0.0f, 0.0f}  // 设置背景颜色为黑色（可选）
        });

    // 创建一个实体，添加 EcsBox 组件
    ecs_entity_t e = ecs_new(world);
 
    
    ecs_set(world, e, EcsRectangle, {
       .width = 50.0f,
       .height = 50.0f,
 
        });



    // 设置位置、旋转、缩放
    ecs_set(world, e, EcsPosition3, { .x = 0.0f, .y = 0.0f, .z = 0.0f });
    ecs_set(world, e, EcsRotation3, { .x = 0.0f, .y = 0.0f, .z = 0.0f });

    ecs_set(world, e, EcsVelocity3, { 0.0, 0.0, 0.0 });
    ecs_set(world, e, EcsRotation3, { GLM_PI / 2.0, 0, 0 });

    ecs_set(world, e, EcsScale3, { .x = 1.0f, .y = 1.0f, .z = 1.0f });

 


    // 设置颜色组件（假设存在 EcsStrokeColor 组件）
    ecs_set(world, e, EcsStrokeColor, { .r = 1.0f, .g = 0.0f, .b = 0.0f });

    // 进展世界，触发初始化系统
    ecs_progress(world, 0);
}





void _sg_shutdown() {
    ecs_fini(world);
}



void _sg_render(int w, int h) {
    app_width = w;
    app_height = h;

 

    // 更新世界
    if (!ecs_progress(world, 0)) {
        printf("ecs_progress error!\n");
    }
}

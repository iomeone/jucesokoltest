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


void _sg_initialize(int w, int h) {

    app_width = w;
    app_height = h;

    dir_of_etc_sokol_shaders = "D:/etc/jucesokoltest/Source/";

    world = ecs_init();

    // 导入必要的模块
    ECS_IMPORT(world, FlecsSystemsSokol);
    ECS_IMPORT(world, FlecsComponentsGraphics);
    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsGeometry);
    ecs_log_set_level(-1);

    // 创建摄像机实体并设置 EcsCamera 组件
    ecs_entity_t camera = ecs_new(world);
    ecs_set(world, camera, EcsCamera, {
        .position = {0.0f, 0.0f, -5.0f},  // 摄像机位置
        .lookat = {0.0f, 0.0f, 0.0f},     // 摄像机注视点
        .up = {0.0f, 1.0f, 0.0f},         // 摄像机上方向
        .fov = 60.0f,                     // 视野角度（以度为单位）
        .near_ = 0.1f,                    // 近裁剪面
        .far_ = 1000.0f,                  // 远裁剪面
        .ortho = false                    // 是否使用正交投影（false 表示透视投影）
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
    ecs_entity_t square = ecs_new(world);

    // 设置位置、旋转、缩放
    ecs_set(world, square, EcsPosition3, { .x = 0.0f, .y = 0.0f, .z = 0.0f });
    ecs_set(world, square, EcsRotation3, { .x = 0.0f, .y = 0.0f, .z = 0.0f });
    ecs_set(world, square, EcsScale3, { .x = 1.0f, .y = 1.0f, .z = 1.0f });

    // 设置 EcsBox 组件，定义正方形的宽度、高度和深度
    ecs_set(world, square, EcsBox, {
        .width = 10.0f,
        .height = 10.0f,
        .depth = 0.0f  // 2D 情况下深度为 0
        });

    // 设置颜色组件（假设存在 EcsStrokeColor 组件）
    ecs_set(world, square, EcsStrokeColor, { .r = 1.0f, .g = 0.0f, .b = 1.0f });

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

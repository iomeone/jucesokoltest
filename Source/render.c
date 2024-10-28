#include "render.h"
#include "flecs.h"
#include "flecs_systems_sokol.h"
#include "flecs_components_graphics.h"
#include "flecs_components_transform.h"
#include "flecs_components_geometry.h"



//https://github.com/flecs-hub/flecs-systems-sokol
//https://github.com/SanderMertens/bouncing-boxes


#define BOX_BRIGHTNESS (2.5)
#define BOX_X_COUNT (400)
#define BOX_Y_COUNT (400)
#define BOX_SIZE (2.0f)



extern char* dir_of_etc_sokol_shaders;




bool dice(float chance) {
    return (float)rand() > ((float)RAND_MAX / chance);
}

float randf() {
    return (float)rand() / (float)RAND_MAX;
}

void Bounce(ecs_iter_t* it) {

   
    EcsPosition3* p = ecs_field(it, EcsPosition3, 0);
    EcsVelocity3* v = ecs_field(it, EcsVelocity3, 1);
    EcsRgb* c = ecs_field(it, EcsRgb, 2);

    for (int i = 0; i < it->count; i++) {
        c[i].r = 4.0;// BOX_BRIGHTNESS* (d ? 1.0f : 0.0f);
        c[i].b = 0.0;// BOX_BRIGHTNESS * 1.0f;
        c[i].g = 0.0;// BOX_BRIGHTNESS* (1.0f - d * 0.2f);
    }


    ///* Fade out colors, and move squares back into position */
    //for (int i = 0; i < it->count; i++) {
    //    float y = p[i].y;
    //    float lt = y < 0;
    //    p[i].y = lt * (y + it->delta_time * v[i].y);
    //    v[i].y += 20 * it->delta_time;

    //    c[i].r = fmaxf(0.96f * c[i].r, 0.0f);
    //    c[i].g = fmaxf(0.93f * c[i].g, 0.0f);
    //    c[i].b = fmaxf(0.99f * c[i].b, 0.0f);
    //}

    ///* Bounce random squares. Lower threshold to increase number of bounces */
    //int threshold = RAND_MAX - RAND_MAX / 1000;

    //for (int i = 0; i < it->count; i++) {
    //    if (rand() > threshold)
    //    {
    //        p[i].y = -0.01f;
    //        v[i].y = -1.0f - 5.0f * randf();

    //        bool d = dice(1.1f);
    //        c[i].r = 8.0;// BOX_BRIGHTNESS* (d ? 1.0f : 0.0f);
    //        c[i].b = 0.0;// BOX_BRIGHTNESS * 1.0f;
    //        c[i].g = 0.0;// BOX_BRIGHTNESS* (1.0f - d * 0.2f);
    //    }
    //}
}



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
 
    ecs_log_set_level(0);
    //setup_custom_logging("flecs_log.txt");
    world = ecs_init();



    // 导入必要的模块

    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsGeometry);
    ECS_IMPORT(world, FlecsComponentsPhysics);
    ECS_IMPORT(world, FlecsComponentsGraphics);
    ECS_IMPORT(world, FlecsComponentsGui);
    ECS_IMPORT(world, FlecsSystemsSokol);
    ECS_IMPORT(world, FlecsGame);


  


    ECS_SYSTEM(world, Bounce, EcsOnUpdate,
        flecs.components.transform.Position3,
        flecs.components.physics.Velocity3,
        flecs.components.graphics.Rgb);




    ecs_entity_t camera = ecs_new(world);

    /* 设置摄像机的注视点为原点 */
    ecs_set(world, camera, EcsCamera, {
        .lookat = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fov = 90.0f,
        .near_ = 0.1f,
        .far_ = 1000.0f,
        .ortho = false,
        });
    ecs_add(world, camera, EcsCameraController);
    ecs_set(world, camera, EcsPosition3, { 0.0f, 0.0f, -5.0f });
    ecs_set(world, camera, EcsRotation3, { .x = 0.0f, .y = 0.f, .z = 0.0f });

    ecs_entity_t canvas = ecs_new(world);

    ecs_set(world, canvas, EcsCanvas, {
        .title = "Flecs Bouncing Boxes",
        .width = w, .height = h,
        .background_color = {0.0f, 0.0f, 0.0f},
        .camera = camera,
        //.fog_density = 5.0f
        });

  


    /* 创建单个盒子实体 */
    ecs_entity_t e = ecs_new(world);

    /* 设置盒子的尺寸 */
    ecs_set(world, e, EcsBox, {
        .width = BOX_SIZE,
        .height = BOX_SIZE ,
        .depth = BOX_SIZE
        });

    /* 设置盒子的颜色 */
    ecs_set(world, e, EcsRgb, { 0.0f, 0.0f, 0.0f });

    /* 设置盒子的位置为中心点 */
    ecs_set(world, e, EcsPosition3, {
        0.0f,  // x 坐标
        0.0f,  // y 坐标
        0.0f   // z 坐标
        });

    /* 设置盒子的速度（可选，如果需要的话） */
    ecs_set(world, e, EcsVelocity3, { 0.0f, 0.0f, 0.0f });





}


void _sg_shutdown() {

}



void _sg_render(int w, int h) {
    app_width = w;
    app_height = h;

 

    // 更新世界
    if (!ecs_progress(world, 0)) {
        printf("ecs_progress error!\n");
    }
}

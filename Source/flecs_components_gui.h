#ifndef FLECS_COMPONENTS_GUI_H
#define FLECS_COMPONENTS_GUI_H

/* This generated file contains includes for project dependencies */


#include <flecs_components_graphics.h>

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef FLECS_COMPONENTS_GUI_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif


ECS_STRUCT(EcsCanvas, {
    char* title;
    int32_t width;
    int32_t height;
    ecs_entity_t camera;
    ecs_entity_t directional_light;
    EcsRgb background_color;
    EcsRgb ambient_light;
    float fog_density;
    });

#ifdef __cplusplus
extern "C" {
#endif

    
        void FlecsComponentsGuiImport(
            ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {
    namespace components {

        class gui {
        public:
            struct Canvas : EcsCanvas {
                Canvas() {
                    this->title = nullptr;

                    this->ambient_light.r = 1.0;
                    this->ambient_light.g = 1.0;
                    this->ambient_light.b = 1.0;

                    this->background_color.r = 0.0;
                    this->background_color.g = 0.0;
                    this->background_color.b = 0.0;

                    this->camera = 0;
                    this->directional_light = 0;
                }
            };

            gui(flecs::world& ecs) {
                // Load module contents
                FlecsComponentsGuiImport(ecs);

                // Bind C++ types with module contents
                ecs.module<flecs::components::gui>();
                ecs.component<Canvas>();
            }
        };

    }
}

#endif
#endif

#endif
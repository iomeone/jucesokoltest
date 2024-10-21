#ifndef FLECS_SYSTEMS_SOKOL_H
#define FLECS_SYSTEMS_SOKOL_H

/* This generated file contains includes for project dependencies */
#include <flecs.h>
#include <flecs_components_gui.h>
#include <flecs_components_input.h>
#include <flecs_components_graphics.h>
#include <flecs_components_transform.h>
#include <flecs_components_geometry.h>
#include <flecs_systems_transform.h>
#include <flecs_game.h>


#ifndef flecs_systems_sokol_STATIC
#if defined(flecs_systems_sokol_EXPORTS) && (defined(_MSC_VER) || defined(__MINGW32__))
#define FLECS_SYSTEMS_SOKOL_API __declspec(dllexport)
#elif defined(flecs_systems_sokol_EXPORTS)
#define FLECS_SYSTEMS_SOKOL_API __attribute__((__visibility__("default")))
#elif defined(_MSC_VER)
#define FLECS_SYSTEMS_SOKOL_API __declspec(dllimport)
#else
#define FLECS_SYSTEMS_SOKOL_API
#endif
#else
#define FLECS_SYSTEMS_SOKOL_API
#endif



#ifdef __cplusplus
extern "C" {
#endif

FLECS_SYSTEMS_SOKOL_API
void FlecsSystemsSokolImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {
namespace systems {

class sokol {
public:
    sokol(flecs::world& ecs) {
        // Load module contents
        FlecsSystemsSokolImport(ecs);

        // Bind C++ types with module contents
        ecs.module<flecs::systems::sokol>();
    }
};

}
}

#endif
#endif

#endif

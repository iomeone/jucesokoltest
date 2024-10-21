#ifndef FLECS_COMPONENTS_CGLM_H
#define FLECS_COMPONENTS_CGLM_H

/* This generated file contains includes for project dependencies */
 
#include "flecs.h"
#include <cglm.h>

#ifdef __cplusplus
extern "C" {
#endif

extern ECS_COMPONENT_DECLARE(vec3);

extern ECS_COMPONENT_DECLARE(vec4);

void FlecsComponentsCglmImport(
            ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif
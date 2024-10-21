#ifndef FLECS_COMPONENTS_TRANSFORM_H
#define FLECS_COMPONENTS_TRANSFORM_H

#include "flecs_components_cglm.h"

 
 

// Define the components used in the transformation
ECS_STRUCT(EcsPosition2, {
    float x;
    float y;
    });

ECS_STRUCT(EcsPosition3, {
    float x;
    float y;
    float z;
    });

ECS_STRUCT(EcsScale2, {
    float x;
    float y;
    });

ECS_STRUCT(EcsScale3, {
    float x;
    float y;
    float z;
    });

ECS_STRUCT(EcsRotation2, {
    float angle;
    });

ECS_STRUCT(EcsRotation3, {
    float x;
    float y;
    float z;
    });

ECS_STRUCT(EcsQuaternion, {
    float x;
    float y;
    float z;
    float w;
    });

extern ECS_COMPONENT_DECLARE(EcsTransform2);

typedef struct EcsTransform2 {
    mat3 value;
} EcsTransform2;

extern ECS_COMPONENT_DECLARE(EcsTransform3);

typedef struct EcsTransform3 {
    mat4 value;
} EcsTransform3;

extern ECS_COMPONENT_DECLARE(EcsProject2);

typedef struct EcsProject2 {
    mat3 value;
} EcsProject2;

extern ECS_COMPONENT_DECLARE(EcsProject3);

typedef struct EcsProject3 {
    mat4 value;
} EcsProject3;

#ifdef __cplusplus
extern "C" {
#endif

    void FlecsComponentsTransformImport(
        ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#endif // FLECS_COMPONENTS_TRANSFORM_H

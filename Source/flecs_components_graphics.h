#ifndef FLECS_COMPONENTS_GRAPHICS_H
#define FLECS_COMPONENTS_GRAPHICS_H

#include "flecs_components_cglm.h"

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef FLECS_COMPONENTS_GRAPHICS_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

    
        ECS_STRUCT(EcsCamera, {
            vec3 position;
            vec3 lookat;
            vec3 up;
            float fov;
            float near_;
            float far_;
            bool ortho;
            });

    
        ECS_STRUCT(EcsDirectionalLight, {
            vec3 position;
            vec3 direction;
            vec3 color;
            float intensity;
            });

    
        ECS_STRUCT(EcsLookAt, {
            float x;
            float y;
            float z;
            });

    
        ECS_STRUCT(EcsRgb, {
            float r;
            float g;
            float b;
            });

    typedef EcsRgb ecs_rgb_t;

    
        ECS_STRUCT(EcsRgba, {
            float r;
            float g;
            float b;
            float a;
            });

    typedef EcsRgba ecs_rgba_t;

    
        ECS_STRUCT(EcsSpecular, {
            float specular_power;
            float shininess;
            });

    
        ECS_STRUCT(EcsEmissive, {
            float value;
            });

    
        ECS_STRUCT(EcsLightIntensity, {
            float value;
            });

    
        ECS_STRUCT(EcsAtmosphere, {
            float intensity;
            float planet_radius;
            float atmosphere_radius;
            vec3 rayleigh_coef;
            float mie_coef;
            float rayleigh_scale_height;
            float mie_scale_height;
            float mie_scatter_dir;
            });

    
        extern ECS_TAG_DECLARE(EcsSun);

    
        void FlecsComponentsGraphicsImport(
            ecs_world_t* world);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#ifndef FLECS_NO_CPP

namespace flecs {
    namespace components {

        class graphics {
        public:
            struct rgb_t : ecs_rgb_t {
                operator float* () {
                    return reinterpret_cast<float*>(this);
                }
            };

            struct rgba_t : ecs_rgba_t {
                operator float* () {
                    return reinterpret_cast<float*>(this);
                }
            };

            struct Camera : EcsCamera {
                Camera() {
                    this->set_position(0, 0, 0);
                    this->set_lookat(0, 1, 1);
                    this->set_up(0, -1, 0);
                    this->set_fov(30);
                    this->near_ = 0.1f;
                    this->far_ = 100;
                    this->ortho = false;
                }

                void set_position(float x, float y, float z) {
                    this->position[0] = x;
                    this->position[1] = y;
                    this->position[2] = z;
                }

                void set_lookat(float x, float y, float z) {
                    this->lookat[0] = x;
                    this->lookat[1] = y;
                    this->lookat[2] = z;
                }

                void set_up(float x, float y, float z) {
                    this->up[0] = x;
                    this->up[1] = y;
                    this->up[2] = z;
                }

                void set_fov(float value) {
                    this->fov = value;
                }
            };

            struct DirectionalLight : EcsDirectionalLight {
                DirectionalLight() {
                    this->set_position(0, 0, 0);
                    this->set_direction(0, 1, 1);
                    this->set_color(1, 1, 1);
                }

                void set_position(float x, float y, float z) {
                    this->position[0] = x;
                    this->position[1] = y;
                    this->position[2] = z;
                }

                void set_direction(float x, float y, float z) {
                    this->direction[0] = x;
                    this->direction[1] = y;
                    this->direction[2] = z;
                }

                void set_color(float r, float g, float b) {
                    this->color[0] = r;
                    this->color[1] = g;
                    this->color[2] = b;
                }
            };

            using Rgb = EcsRgb;
            using Rgba = EcsRgba;
            using Specular = EcsSpecular;
            using Emissive = EcsEmissive;

            graphics(flecs::world& ecs) {
                // Load module contents
                FlecsComponentsGraphicsImport(ecs);

                // Bind C++ types with module contents
                ecs.module<flecs::components::graphics>();
                ecs.component<Camera>();
                ecs.component<DirectionalLight>();
                ecs.component<Rgb>();
                ecs.component<Rgba>();
                ecs.component<Specular>();
                ecs.component<Emissive>();
            }
        };

    }
}

#endif
#endif

#endif
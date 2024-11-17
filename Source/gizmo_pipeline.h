#pragma once

#include "sokol_gfx.h"

#include "common.h"




class GizmoPipeline {
public:
    GizmoPipeline();
    ~GizmoPipeline();

    sg_pipeline pipeline;
    sg_shader shader;
};
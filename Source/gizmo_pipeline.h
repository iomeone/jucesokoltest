#pragma once

#include "sokol_gfx.h"

#include "common.h"




class GizmoPipeline {
public:
    GizmoPipeline();
    void release();

    sg_pipeline pipeline;
    sg_shader shader;
};
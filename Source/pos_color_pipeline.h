#pragma once

#include "sokol_gfx.h"




struct SimplePipeline
{


    sg_pass pass;
    sg_pipeline pipeline;
    sg_bindings bindings;

    sg_buffer vertex_buffer;

    SimplePipeline(void);

};


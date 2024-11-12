#pragma once

#include "sokol_gfx.h"




struct SimplePipeline
{


    sg_pass pass;
    sg_pipeline pipeline;

    sg_pipeline pipeline_use_index;


    SimplePipeline(void);

};


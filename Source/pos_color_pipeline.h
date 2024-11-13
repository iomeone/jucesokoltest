#pragma once

#include "sokol_gfx.h"


#include "glm/glm.hpp"


struct vs_params_t
{
    glm::mat4 view_proj;
    //glm::mat4 model;
};


struct SimplePipeline
{


    sg_pass pass;
    sg_pipeline pipeline;

    sg_pipeline pipeline_use_index;

    SimplePipeline(sg_primitive_type primitive_type = _SG_PRIMITIVETYPE_DEFAULT);

};


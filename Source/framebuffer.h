#pragma once

#include "sokol_gfx.h"


struct Framebuffer
{
    sg_pass pass;
    sg_pipeline pipeline;
    sg_bindings bindings;
    sg_image color;
    sg_image depth;
    sg_buffer vertex_buffer;

    Framebuffer(void);
};

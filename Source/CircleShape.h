#pragma once

#include "sokol_gfx.h"

class CircleShape {
public:
    static CircleShape& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetNumElements() const { return vertex_count; }
    void release();
private:
    CircleShape();
 

    CircleShape(const CircleShape&) = delete;
    CircleShape& operator=(const CircleShape&) = delete;

    sg_bindings bindings;
    sg_buffer vertex_buffer;
    int vertex_count;
};

#pragma once

#include "sokol_gfx.h"

class EnneperSurface {
public:
    static EnneperSurface& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetIndexCount() const { return index_count; }

private:
    EnneperSurface();
    ~EnneperSurface() = default;

    EnneperSurface(const EnneperSurface&) = delete;
    EnneperSurface& operator=(const EnneperSurface&) = delete;

    sg_bindings bindings;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;
    int index_count;
};

#pragma once

#include "sokol_gfx.h"

class RoseCurve {
public:
    static RoseCurve& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetNumElements() const { return vertex_count; }
    void release();
private:
    RoseCurve();

    RoseCurve(const RoseCurve&) = delete;
    RoseCurve& operator=(const RoseCurve&) = delete;

    sg_bindings bindings;
    sg_buffer vertex_buffer;
    int vertex_count;
};

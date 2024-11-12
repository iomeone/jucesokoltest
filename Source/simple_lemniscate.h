#pragma once

#include "sokol_gfx.h"


//https://paulbourke.net/geometry/lemniscape/
class SimpleLemniscate {
public:
    static SimpleLemniscate& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetNumElements() const { return num_elements; }


    //call in _sg_shutdown
    void release();
private:
    SimpleLemniscate();

    SimpleLemniscate(const SimpleLemniscate&) = delete;
    SimpleLemniscate& operator=(const SimpleLemniscate&) = delete;

    sg_bindings bindings;
    sg_buffer vertex_buffer;
    sg_buffer index_buffer;

    int num_elements; 
};

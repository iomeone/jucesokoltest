#pragma once

#include "sokol_gfx.h"





class SimpleQuad {
public:
 
    static SimpleQuad& Instance();

 
    const sg_bindings& GetBindings() const { return bindings; }

    const sg_bindings& GetBindings_use_index() const { return bindings_use_index; }


    int GetNumElements() const { return 3; }

    void release();
private:
 
    SimpleQuad();
    ~SimpleQuad() = default;

   
    SimpleQuad(const SimpleQuad&) = delete;
    SimpleQuad& operator=(const SimpleQuad&) = delete;

  
    sg_bindings bindings;
    sg_buffer vertex_buffer;



    sg_buffer index_buffer;
    sg_bindings bindings_use_index;
};

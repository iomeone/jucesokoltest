#pragma once

#include "sokol_gfx.h"





class SimpleQuad {
public:
 
    static SimpleQuad& Instance();

 
    const sg_bindings& GetBindings() const { return bindings; }

private:
 
    SimpleQuad();
    ~SimpleQuad() = default;

   
    SimpleQuad(const SimpleQuad&) = delete;
    SimpleQuad& operator=(const SimpleQuad&) = delete;

  
    sg_bindings bindings;
    sg_buffer vertex_buffer;
};

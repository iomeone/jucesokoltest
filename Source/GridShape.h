#pragma once

#include "sokol_gfx.h"

class GridShape {
public:
    static GridShape& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetNumElements() const { return num_elements; }

    // Method to rebuild the grid with new parameters
    void RebuildGrid(float xfrom = -40.0f, float xto = 40.0f, float xstep = 4.0f,
        float zfrom = -40.0f, float zto = 40.0f, float zstep = 4.0f);

    void release();

private:
    GridShape();


    GridShape(const GridShape&) = delete;
    GridShape& operator=(const GridShape&) = delete;

    sg_bindings bindings{};
    sg_buffer vertex_buffer{};
    int num_elements{ 0 };

    // Grid parameters
    float xfrom_;
    float xto_;
    float xstep_;
    float zfrom_;
    float zto_;
    float zstep_;

    // Helper method to generate the grid vertices
    void GenerateGridVertices();
};

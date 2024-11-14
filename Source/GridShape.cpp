#include "GridShape.h"
#include <vector>
#include <cmath>

GridShape& GridShape::Instance() {
    static GridShape instance;
    return instance;
}

GridShape::GridShape()
    : xfrom_(-40.0f), xto_(40.0f), xstep_(4.0f),
    zfrom_(-40.0f), zto_(40.0f), zstep_(4.0f) {
    GenerateGridVertices();
}

void GridShape::release() {
    sg_destroy_buffer(vertex_buffer);
}

void GridShape::RebuildGrid(float xfrom, float xto, float xstep,
    float zfrom, float zto, float zstep) {
    // Update parameters
    xfrom_ = xfrom;
    xto_ = xto;
    xstep_ = xstep;
    zfrom_ = zfrom;
    zto_ = zto;
    zstep_ = zstep;

    // Destroy the old buffer
    sg_destroy_buffer(vertex_buffer);

    // Generate new vertices and create a new buffer
    GenerateGridVertices();
}

void GridShape::GenerateGridVertices() {
    std::vector<float> vertices;

    // Generate grid lines along the x-axis (lines parallel to the z-axis)
    for (float x = xfrom_; x <= xto_; x += xstep_) {
        // Line from (x, 0, zfrom_) to (x, 0, zto_)
        // First vertex
        vertices.push_back(x);          // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(zfrom_);     // z

        vertices.push_back(0.5f);       // r
        vertices.push_back(0.5f);       // g
        vertices.push_back(0.5f);       // b
        vertices.push_back(1.0f);       // a

        // Second vertex
        vertices.push_back(x);          // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(zto_);       // z

        vertices.push_back(0.5f);       // r
        vertices.push_back(0.5f);       // g
        vertices.push_back(0.5f);       // b
        vertices.push_back(1.0f);       // a
    }

    // Generate grid lines along the z-axis (lines parallel to the x-axis)
    for (float z = zfrom_; z <= zto_; z += zstep_) {
        // Line from (xfrom_, 0, z) to (xto_, 0, z)
        // First vertex
        vertices.push_back(xfrom_);     // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(z);          // z

        vertices.push_back(0.5f);       // r
        vertices.push_back(0.5f);       // g
        vertices.push_back(0.5f);       // b
        vertices.push_back(1.0f);       // a

        // Second vertex
        vertices.push_back(xto_);       // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(z);          // z

        vertices.push_back(0.5f);       // r
        vertices.push_back(0.5f);       // g
        vertices.push_back(0.5f);       // b
        vertices.push_back(1.0f);       // a
    }

    num_elements = static_cast<int>(vertices.size() / 7); // Each vertex has 7 floats

    // Create the vertex buffer
    sg_buffer_desc buffer_desc = {
        .data = {
            .ptr = vertices.data(),
            .size = vertices.size() * sizeof(float)
        },
        .label = "circle-vertices",
    };

    vertex_buffer = sg_make_buffer(&buffer_desc);

    bindings = {
        .vertex_buffers[0] = vertex_buffer,
    };
}

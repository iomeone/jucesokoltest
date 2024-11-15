#pragma once

#include "sokol_gfx.h"

#include <vector>
#include <glm/glm.hpp>


class CircleShape {
public:
    static CircleShape& Instance();

    const sg_bindings& GetBindings() const { return bindings; }
    int GetNumElements() const { return vertex_count; }

    int GetNumInstances() const { return instance_count; }


    void SetTranslation(const glm::vec3& translation);
    void SetTranslations(const std::vector<glm::vec3>& translations);


    void release();
private:
    CircleShape();
 

    CircleShape(const CircleShape&) = delete;
    CircleShape& operator=(const CircleShape&) = delete;

    sg_bindings bindings;
    sg_buffer vertex_buffer;
    sg_buffer instance_buffer; // Instance buffer for transformation matrices

    int vertex_count;
    int instance_count = 1; // Number of instances

    std::vector<glm::mat4> transform_matrices; // Transformation matrices

};

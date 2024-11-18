#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


 //camera_quaternion::camera_quaternion() {
 //	m_position = glm::vec3(0, 5, 10);
 //	m_orientation = glm::identity<glm::quat>();// glm::fquat(1, 0, 0, 0);
 //	m_fov = 57.295779513f;
 //	m_aspect = 4.0f/3.0f;
 //	m_nearPlane = 0.01f;
 //	m_farPlane = 100.0f;
 //
 //}


class camera_quaternion
{
public:

    glm::vec3 m_xaxis;                  // camera_quaternion view matrix x axis
    glm::vec3 m_yaxis;                  // camera_quaternion view matrix y axis
    glm::vec3 m_zaxis;                  // camera_quaternion view matrix z axis

    glm::vec3 forward;                  // Where the camera is pointing.
    glm::vec3 fpforward;               // First person forwards direction.

    float aspect;                       // The camera aspect ratio (width/height).
    float fovy;                         // The camera vertical field of view.
    float zNear;                        // The camera view frustrum near clipping plane.
    float zFar;                         // The camera view frustrum far clipping plane.

public:


    
    glm::vec3 position;
    glm::vec3 rotSpeed;
    glm::vec3 movSpeed;

    // camera_quaternion matrices
    glm::fquat orientation;             // The camera's current orientation quaternion
    glm::mat4 projection;               // The camera projection (perspective) matrix.
    glm::mat4 view;                     // The camera view matrix.
    glm::mat4 mvp;                      // The camera modelview projection matrix.
    glm::mat4 modelview;                // Modelview matrix.
    camera_quaternion();
    void update(float);
    void setMVP();
    void perspective();
    void setPos(float, float, float);
    float getPosX();
    float getPosY();
    float getPosZ();
    void setFlightMode(bool);
    glm::fquat orient(float, float);
    glm::vec3 move(float, float, float);
    bool visibleY();
    bool visibleZ();
    glm::vec3 getCameraUp();            // Get the camera UP vector.
    glm::vec3 getCameraRight();         // Get the camera RIGHT vector.
    glm::vec3 getCameraForward();       // Get the camera FORWARD vector.
    glm::vec3 getCameraFPForward();     // Get the first person forward vector.


    glm::mat4 projectionViewMatrix() const;

    glm::mat4 viewMatrix()
    {
        return glm::mat4_cast(orientation);
    }

    glm::mat4 projectionMatrix() const
    {
        auto projection = glm::perspective(fovy, aspect, zNear, zFar);
        return projection;
    }

    void setAspectRatio(float as)
    {
        aspect = as;
    }
private:
    glm::fquat fromAxisAngle(glm::vec3, float);
    void updateView();
    bool flightMode;                    // Switch to enable camera flight mode.
};
#pragma once

// glm
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"
#include <JuceHeader.h>
#include "linalg.h"
#include "Event.h"
namespace batteries
{
  class Camera
  {
  public:
    Camera();

    glm::mat4 View() const;
    glm::mat4 Projection() const;

  public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 center;

  public:
    bool orthographic = false;
    float orthoHeight = 6.0f;

    float fov = 60.0f;
    float nearz = 0.01f;
    float farz = 100.0f;
  };

  struct camera_desc
  {
    int mode;
    float distance;
    float yaw;
    float pitch;
  };

  class CameraController
  {
  public:
    enum class Mode : int
    {
      Free = 0,
      Orbit = 1,
    };

  public:
    CameraController() = default;

    void Configure(const camera_desc &desc);
    void SetCamera(Camera *camera);
    void SetMode(const Mode mode);

    void Update(float dt);
    void Event(TKeyEvent& key);
    void Debug(void);


    linalg::aliases::float4 get_orientation() const
    {
        return linalg::qmul(linalg::rotation_quat(linalg::aliases::float3(0, 1, 0), yaw), linalg::rotation_quat(linalg::aliases::float3(1, 0, 0), pitch));
    }

  private:
    Camera *camera;

    bool ismouseDown = false;
    // control options
    float movement_speed = 5.0f;
    float smoothing_factor = 1.0f;
    float t = 0.0f;

    Mode mode = Mode::Free;

    float min_pitch = -89.0f;
    float max_pitch = +89.0f;
    float min_dist = 2.0f;
    float max_dist = 30.0f;
    float distance = 5.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float zoom = 45.0f;
    bool move_forward = false;
    bool move_backward = false;
    bool move_left = false;
    bool move_right = false;
    bool move_up = false;
    bool move_down = false; 

    struct
    {
      float dampening = 0.25f;
    } settings;
  };
}
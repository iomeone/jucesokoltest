#include "camera.h"

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
//#include "glm/glm.hpp"

// imgui
//#include "imgui/imgui.h"

#include <algorithm>

namespace
{
  constexpr glm::vec3 world_up{0.0f, 1.0f, 0.0f};

  static glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float t)
  {
    return a + t * (b - a);
  }

  static glm::vec3 euclidean(float yaw, float pitch)
  {
    const float y = glm::radians(yaw);
    const float p = glm::radians(pitch);
    return {cosf(p) * cosf(y), sinf(p), cosf(p) * sinf(y)};
  }
}

namespace batteries
{
  Camera::Camera()
      : position{0.0f, 0.0f, 10.0f}, front{0.0f, 0.0f, -1.0f}, up{0.0f, 1.0f, 0.0f}, right{1.0f, 0.0f, 0.0f}, center{0.0f, 0.0f, 0.0f}
  {
  }

  glm::mat4 Camera::View() const
  {
    return glm::lookAt(position, center, up);
  }

  glm::mat4 Camera::Projection(float w, float h) const
  {
    float aspect = w / h;
    return orthographic
               ? glm::ortho(orthoHeight, fov, nearz, farz)
               : glm::perspective(glm::radians(fov), aspect, nearz, farz);
  }

  void CameraController::SetCamera(Camera *camera)
  {
    this->camera = camera;
  }

  void CameraController::SetMode(const Mode mode)
  {
    this->mode = mode;
  }

  void CameraController::Configure(const camera_desc &desc)
  {
    distance = desc.distance;
    yaw = desc.yaw;
    pitch = desc.pitch;
  }

  void CameraController::Update(float dt)
  {
    switch (mode)
    {
    case Mode::Free:
    {
      auto velocity = movement_speed * dt;
      if (move_forward)
      {
        camera->position += camera->front * velocity;
      }
      if (move_backward)
      {
        camera->position -= camera->front * velocity;
      }
      if (move_left)
      {
        camera->position -= camera->right * velocity;
      }
      if (move_right)
      {
        camera->position += camera->right * velocity;
      }

      if (move_up)
      {
          camera->position += camera->up * velocity;
      }
      if (move_down)
      {
          camera->position -= camera->up * velocity;
      }


      camera->front = euclidean(yaw, pitch);
      camera->right = glm::normalize(glm::cross(camera->front, {0.0f, 1.0f, 0.0f}));
      camera->up = glm::normalize(glm::cross(camera->right, camera->front));
      camera->center = camera->position + camera->front;
      break;
    }
    case Mode::Orbit:
    {
      camera->center = {0.0f, 0.0f, 0.0f};
      camera->position = camera->center + euclidean(yaw, pitch) * distance;
      break;
    }
    }
  }


  void CameraController::Event(TKeyEvent& key)
  {
     
      switch (key.type)
      {
      case e_key_down:
          if (key.key_code == 'W' || key.key_code == juce::KeyPress::upKey)
          {
              move_forward = true;
          }
          else if (key.key_code == 'S' || key.key_code == juce::KeyPress::downKey)
          {
              move_backward = true;
          }
          else if (key.key_code == 'A' || key.key_code == juce::KeyPress::leftKey)
          {
              move_left = true;
          }
          else if (key.key_code == 'D' || key.key_code == juce::KeyPress::rightKey)
          {
              move_right = true;
          }
          else if (key.key_code == 'Q' || key.key_code == 'q')
          {
              move_down = true;
          }
          else if (key.key_code == 'E' || key.key_code == 'e')
          {
              move_up = true;
          }
          break;
      case e_key_up:
          if (key.key_code == 'W' || key.key_code == juce::KeyPress::upKey)
          {
              move_forward = false;
          }
          else if (key.key_code == 'S' || key.key_code == juce::KeyPress::downKey)
          {
              move_backward = false;
          }
          else if (key.key_code == 'A' || key.key_code == juce::KeyPress::leftKey)
          {
              move_left = false;
          }
          else if (key.key_code == 'D' || key.key_code == juce::KeyPress::rightKey)
          {
              move_right = false;
          }
          else if (key.key_code == 'Q' || key.key_code == 'q')
          {
              move_down = false;
          }
          else if (key.key_code == 'E' || key.key_code == 'e')
          {
              move_up = false;
          }
          break;

     case e_mouse_down:
      if (key.mouse_button == e_mouse_left)
      {
          ismouseDown = true;
      }
      break;
    case e_mouse_up:
      if (key.mouse_button == e_mouse_left)
      {
          ismouseDown = false;
      }
      break;
    case e_mouse_scroll:
      distance = glm::clamp(min_dist, distance + (key.scroll_y * 0.5f), max_dist);
      camera->position += camera->front * key.scroll_y;
      break;
    case e_mouse_move:
      if (ismouseDown)
      {
        yaw += key.mouse_dx * settings.dampening;
        pitch -= key.mouse_dy * settings.dampening;
        pitch = glm::clamp(min_pitch, pitch, max_pitch);
      }
      break;
    default:
      break;
    }
  }

  void CameraController::Debug(void)
  {
    //ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    //ImGui::Text("Position: %.2f, %.2f, %.2f", camera->position.x, camera->position.y, camera->position.z);
    //ImGui::Text("Pitch: %.2f", pitch);
    //ImGui::Text("Yaw: %.2f", yaw);
    //ImGui::Text("Distance: %.2f", distance);
    //ImGui::SliderFloat("movement_speed", &movement_speed, 0.0f, 100.0f);
    //ImGui::SliderFloat("smoothing_factor", &smoothing_factor, 0.0f, 1.0f);

    //ImGui::End();
  }
}
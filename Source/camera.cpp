#include "camera.h"

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

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
      : position{0.0f, 0.0f, 5.0f}, front{0.0f, 0.0f, -1.0f}, up{0.0f, 1.0f, 0.0f}, right{1.0f, 0.0f, 0.0f}, center{0.0f, 0.0f, 0.0f}
  {
  }

  glm::mat4 Camera::View() const
  {
    return glm::lookAt(position, center, up);
  }

  glm::mat4 Camera::Projection() const
  {
    constexpr float aspect = 800.0f / 600.0f;
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

      camera->front = euclidean(yaw, pitch);
      camera->right = glm::normalize(glm::cross(camera->front, {0.0f, 1.0f, 0.0f}));
      camera->up = glm::normalize(glm::cross(camera->right, camera->front));
      camera->center = camera->position + camera->front;
    }
    case Mode::Orbit:
    {
      camera->center = {0.0f, 0.0f, 0.0f};
      camera->position = camera->center + euclidean(yaw, pitch) * distance;
    }
    }
  }
  /*
  
  
          float cameraSpeed = 0.5f; //
        if (key == 'w' || key == 'W')
        {
            camera.ProcessKeyboard(Camera_Movement::FORWARD, cameraSpeed);

        }
        if (key == 's' || key == 'S')
        {
            camera.ProcessKeyboard(Camera_Movement::BACKWARD, cameraSpeed);
        }
        if (key == 'a' || key == 'A')
        {
            camera.ProcessKeyboard(Camera_Movement::LEFT, cameraSpeed);

        }
        if (key == 'd' || key == 'D')
        {
            camera.ProcessKeyboard(Camera_Movement::RIGHT, cameraSpeed);
        }
        */
  void CameraController::Event(const juce::KeyPress& key)
  {
    switch (e->type)
    {
    case SAPP_EVENTTYPE_KEY_DOWN:
      if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP)
      {
        move_forward = true;
      }
      else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN)
      {
        move_backward = true;
      }
      else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT)
      {
        move_left = true;
      }
      else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT)
      {
        move_right = true;
      }
      break;
    case SAPP_EVENTTYPE_KEY_UP:
      if (e->key_code == SAPP_KEYCODE_W || e->key_code == SAPP_KEYCODE_UP)
      {
        move_forward = false;
      }
      else if (e->key_code == SAPP_KEYCODE_S || e->key_code == SAPP_KEYCODE_DOWN)
      {
        move_backward = false;
      }
      else if (e->key_code == SAPP_KEYCODE_A || e->key_code == SAPP_KEYCODE_LEFT)
      {
        move_left = false;
      }
      else if (e->key_code == SAPP_KEYCODE_D || e->key_code == SAPP_KEYCODE_RIGHT)
      {
        move_right = false;
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_DOWN:
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        sapp_lock_mouse(true);
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_UP:
      if (e->mouse_button == SAPP_MOUSEBUTTON_LEFT)
      {
        sapp_lock_mouse(false);
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_SCROLL:
      distance = glm::clamp(min_dist, distance + (e->scroll_y * 0.5f), max_dist);
      break;
    case SAPP_EVENTTYPE_MOUSE_MOVE:
      if (sapp_mouse_locked())
      {
        yaw += e->mouse_dx * settings.dampening;
        pitch -= e->mouse_dy * settings.dampening;
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
#ifndef camera_h
#define camera_h

// clang-format off
#include "pch.h"
// clang-format on

#include "object3.h"

struct Camera : protected Object3
{
    Camera() noexcept;

    void
    LookAt(const glm::vec3& _target_position, glm::mat4& _view_matrix) noexcept;

    void
    SetPosition(const float&& _x, const float&& _y, const float&& _z) noexcept;

  private:
    void
    UpdateTranslationMatrix() noexcept;

    glm::mat4       ijk_orientation;
    glm::mat4       xyz_translation;
    const glm::mat4 i_mat = glm::mat4(1.0);
};

#endif
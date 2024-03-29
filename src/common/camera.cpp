
// clang-format off
#include "pch.h"
// clang-format on

#include "camera.h"

#include "orientation.h"
#include "position.h"

Camera::Camera() noexcept
{
    xyz_translation = glm::mat4(1.0f);
    UpdateTranslationMatrix();

    ijk_orientation = glm::mat4(1.0f);
};

void
Camera::LookAt(const glm::vec3& _target_position_in, glm::mat4& _view_matrix_out) noexcept
{
    // [ cfarvin::TODO ] Remove these glm:: allocations
    _view_matrix_out = glm::translate(
      i_mat,
      glm::vec3(ijk_orientation * xyz_translation * glm::vec4(_target_position_in, 1.0f)));
}

void
Camera::SetPosition(const float&& _x_in, const float&& _y_in, const float&& _z_in) noexcept
{
    Object3::SetPosition(std::move(_x_in), std::move(_y_in), std::move(_z_in));
    UpdateTranslationMatrix();
}

void
Camera::UpdateTranslationMatrix() noexcept
{
    xyz_translation[3].x = -position.x;
    xyz_translation[3].y = -position.y;
    xyz_translation[3].z = -position.z;
}
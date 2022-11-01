
#include "camera.h"

#include "glm/glm.hpp"
#pragma warning(disable : 4201)
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr and glm::translate
#pragma warning(default : 4201)
#include "orientation.h"
#include "position.h"

Camera::Camera()
{
    xyz_translation = glm::mat4(1.0f);
    UpdateTranslationMatrix();

    ijk_orientation = glm::mat4(1.0f);
    UpdateOrientationMatrix();
};

void
Camera::LookAt(const glm::vec3& _target_position, glm::mat4& _view_matrix)
{
    // [ cfarvin::TODO ] Remove these glm:: allocations
    _view_matrix = glm::translate(
      i_mat,
      glm::vec3(ijk_orientation * xyz_translation * glm::vec4(_target_position, 1.0f)));
}

void
Camera::SetPosition(const float&& _x, const float&& _y, const float&& _z)
{
    Object3::SetPosition(std::move(_x), std::move(_y), std::move(_z));
    UpdateTranslationMatrix();
}

void
Camera::SetOrientation(const float&& _i, const float&& _j, const float&& _k)
{
    Object3::SetOrientation(std::move(_i), std::move(_j), std::move(_k));
    UpdateOrientationMatrix();
}

void
Camera::UpdateTranslationMatrix()
{
    xyz_translation[3].x = -position.x;
    xyz_translation[3].y = -position.y;
    xyz_translation[3].z = -position.z;
}

// [ cfarvin::TODO ] This impl is likely incorrect.
void
Camera::UpdateOrientationMatrix()
{
    ijk_orientation[0].x = orientation.i;
    ijk_orientation[1].y = orientation.j;
    ijk_orientation[2].z = orientation.k;
}
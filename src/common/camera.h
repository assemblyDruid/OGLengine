#ifndef camera_h
#define camera_h

#include "glm/glm.hpp"
#include "object3.h"

struct Camera : protected Object3
{
    Camera();

    void
    LookAt(const glm::vec3& _target_position, glm::mat4& _view_matrix);

    void
    SetPosition(const float&& _x, const float&& _y, const float&& _z);

  private:
    void
    UpdateTranslationMatrix();

    glm::mat4       ijk_orientation;
    glm::mat4       xyz_translation;
    const glm::mat4 i_mat = glm::mat4(1.0);
};

#endif
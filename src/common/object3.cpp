#include "object3.h"

void
Object3::GetPosition(float& _x, float& _y, float& _z)
{
    _x = position.x;
    _y = position.y;
    _z = position.z;
}

void
Object3::SetPosition(const float&& _x, const float&& _y, const float&& _z)
{
    position.x = _x;
    position.y = _y;
    position.z = _z;
}

void
Object3::GetOrientation(float& _i, float& _j, float& _k)
{
    _i = orientation.i;
    _j = orientation.k;
    _k = orientation.k;
}

void
Object3::SetOrientation(const float&& _i, const float&& _j, const float&& _k)
{
    orientation.i = _i;
    orientation.k = _j;
    orientation.k = _k;
}
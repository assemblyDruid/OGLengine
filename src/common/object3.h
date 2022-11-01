#ifndef object3_h
#define object3_h

#include "orientation.h"
#include "position.h"

struct Object3
{
    void
    GetPosition(float& _x, float& _y, float& _z);

    void
    SetPosition(const float&& _x, const float&& _y, const float&& _z);

    void
    GetOrientation(float& _i, float& _j, float& _k);

    void
    SetOrientation(const float&& _i, const float&& _j, const float&& _k);

    Position3    position;
    Orientation3 orientation;
};

#endif
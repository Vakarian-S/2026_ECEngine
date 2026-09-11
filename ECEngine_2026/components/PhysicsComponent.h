#pragma once
#include <Vector.h>

#include "../Component.h"
#include "../TransformComponent.h"

class PhysicsComponent : public TransformComponent
{
public:
    PhysicsComponent(WeakRef<Component> parent_);

    float mass_;
    Vec3 acceleration_;
    Vec3 velocity_;
    Vec3 jerk;
};

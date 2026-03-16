#pragma once
#include <Vector.h>

#include "../Component.h"

class PhysicsComponent : public Component
{
public:
    PhysicsComponent(WeakRef<Component> parent_);
    
    float mass;
    MATH::Vec3 velocity;
    
    
};



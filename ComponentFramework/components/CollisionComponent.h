#pragma once
#include <cstdint>
#include <Plane.h>
#include <Vector.h>

#include "../Component.h"

enum class Collider_type : uint8_t
{
    SPHERE,
    AABB,
    PLANE
};

struct AABB
{
    MATH::Vec3 center;
    MATH::Vec3 halfExtents;
};

class CollisionComponent : public Component
{
protected:
    Collider_type type;
    AABB aabb;
    float radius;
    MATHEX::Plane plane;

public:
    CollisionComponent(WeakRef<Component> parent, float radius);
    CollisionComponent(WeakRef<Component> parent, AABB aabb);
    CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane);

    bool OnCreate() { return true; }

    void OnDestroy()
    {
    }

    void Update(const float deltaTime_)
    {
    }

    void Render() const
    {
    }
};

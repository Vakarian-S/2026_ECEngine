#include "CollisionComponent.h"

CollisionComponent::CollisionComponent(WeakRef<Component> parent, float radius) : Component(parent), radius(radius)
{
}


CollisionComponent::CollisionComponent(WeakRef<Component> parent, AABB aabb) : Component(parent), aabb(aabb),
                                                                               radius(1.0f)
{
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane) : Component(parent),
    plane(plane), radius(1.0f)
{
}

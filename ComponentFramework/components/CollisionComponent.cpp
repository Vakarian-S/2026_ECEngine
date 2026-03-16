#include "CollisionComponent.h"

CollisionComponent::CollisionComponent(WeakRef<Component> parent, float radius) : Component(parent), radius(radius)
{
}


CollisionComponent::CollisionComponent(WeakRef<Component> parent, AABB aabb) : Component(parent), aabb(aabb)
{
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane) : Component(parent), plane(plane)
{
}

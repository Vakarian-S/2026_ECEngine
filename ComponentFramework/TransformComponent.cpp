#include "TransformComponent.h"

#include <MMath.h>


TransformComponent::TransformComponent(Component* parent_): Component(parent_)
{
    position = Vec3(0.0f, 0.0f, 0.0f);
    scale = Vec3(1.0f, 1.0f, 1.0f);
    orientation = Quaternion(1.0f, Vec3(1.0f, 1.0f, 1.0f));
}

TransformComponent::TransformComponent(Component* parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_)
    : Component(parent_), position(pos_), orientation(orientation_), scale(scale_)
{
}

TransformComponent::~TransformComponent()
{
}

bool TransformComponent::OnCreate()
{
    if (isCreated == true) return true;
    isCreated = true;
    return true;
}

void TransformComponent::OnDestroy()
{
}

void TransformComponent::Update(const float deltaTime_)
{
    std::cout << "Hello from update" << deltaTime_ << '\n';
}

void TransformComponent::Render() const
{
}

Matrix4 TransformComponent::GetTransformMatrix() const
{
    return MMath::translate(position) * MMath::scale(scale) * MMath::toMatrix4(orientation);
}

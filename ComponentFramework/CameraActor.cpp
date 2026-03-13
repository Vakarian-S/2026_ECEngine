#include "CameraActor.h"

#include <MMath.h>

#include "TransformComponent.h"

CameraActor::CameraActor(std::weak_ptr<Component> parent, float fieldOfView, float aspectRatio, float near, float far)
    : Actor(parent)
{
    projectionMatrix = MMath::perspective(fieldOfView, aspectRatio, near, far);
    viewMatrix.loadIdentity();
}

CameraActor::~CameraActor()
{
}

bool CameraActor::OnCreate()
{
    Ref transformComponent = GetComponent<TransformComponent>();
    if (transformComponent != nullptr)
    {
        viewMatrix = transformComponent->GetTransformMatrix();
        position = transformComponent->GetPosition();
        orientation = transformComponent->GetQuaternion();
        viewMatrix.print("View Matrix");
    }
    return true;
}

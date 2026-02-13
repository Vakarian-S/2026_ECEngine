#include "CameraActor.h"

#include <MMath.h>

#include "TransformComponent.h"

CameraActor::CameraActor(Actor* parent, float fieldOfView, float aspectRatio, float near, float far)
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
    const TransformComponent* transformComponent = GetComponent<TransformComponent>();
    if (transformComponent != nullptr)
    {
        viewMatrix = transformComponent->GetTransformMatrix();
        viewMatrix.print("View Matrix");
    } 
    return true;
}



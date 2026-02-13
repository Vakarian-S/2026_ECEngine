#pragma once
#include <Matrix.h>
#include <MMath.h>
#include <Quaternion.h>

#include "Actor.h"

class CameraActor : public Actor
{
private:
    MATH::Matrix4 projectionMatrix;
    MATH::Matrix4 viewMatrix;
    MATH::Quaternion orientation;
    MATH::Vec3 position;

public:
    CameraActor(Actor* parent, float fieldOfView, float aspectRatio, float near, float far);
    ~CameraActor();
    bool OnCreate() override;
    MATH::Matrix4 GetProjectionMatrix() { return projectionMatrix; }
    MATH::Quaternion GetOrientation() { return orientation; }

    Vec3 freeCameraMovement(const Vec3& direction) const
    {
        Matrix4 worldToCamera = this->GetViewMatrix();
        Matrix4 cameraToWorld = MMath::inverse(worldToCamera);
        Vec3 rotated_forward_in_cam_space = cameraToWorld * direction;
        return rotated_forward_in_cam_space;
    }

    MATH::Matrix4 GetViewMatrix() const
    {
        return MMath::inverse(MMath::toMatrix4(orientation)) * MMath::inverse(MMath::translate(position));
    }

    void SetView(const MATH::Quaternion& orientation_, const MATH::Vec3& position_)
    {
        orientation = orientation_;
        position = position_;
    }
};

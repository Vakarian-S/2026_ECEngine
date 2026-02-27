#pragma once
#include "Component.h"
#include "Matrix.h"
#include "QMath.h"
#include "Euler.h"
using namespace MATH;

class TransformComponent : public Component
{
private:
    Vec3 position;
    Vec3 scale;
    Quaternion orientation;

public:
    TransformComponent(Ref<Component> parent_);
    TransformComponent(Ref<Component> parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f));
    ~TransformComponent();
    bool OnCreate() override;
    void OnDestroy() override;
    void Update(const float deltaTime_) override;
    void Render() const override;

    Vec3 GetPosition() const { return position; }
    void SetPosition(Vec3 position_) { position = position_; }
    
    Vec3 GetScale() { return scale; }
    
    Quaternion GetQuaternion() { return orientation; }
    void SetQuaternion(Quaternion orientation_) { orientation = orientation_; }
    
    Matrix4 GetTransformMatrix() const;

    void SetTransform(Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f))
    {
        position = pos_;
        orientation = orientation_;
        scale = scale_;
    }
};

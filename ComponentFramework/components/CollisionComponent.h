#pragma once
#include <cstdint>
#include <glew.h>
#include <Plane.h>
#include <Vector.h>
#include <Matrix.h>

#include "../Component.h"
#include "../ShaderComponent.h"

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
    AABB aabb_;
    Collider_type type_;
    float radius_;
    MATHEX::Plane plane_;

    /**
     * Local-space offset applied to the wireframe geometry at build time.
     * Use this to align the collision shape when the mesh origin is at the
     * base/feet rather than the body centre.
     * For AABB this is additive on top of aabb_.center.
     * Must be set before OnCreate() to take effect.
     */
    MATH::Vec3 local_offset_ = MATH::Vec3(0.0f, 0.0f, 0.0f);

    /** Per-instance GPU wireframe geometry **/
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLsizei line_vertex_count_ = 0;

    /**
     * Shared wireframe shader — A single shader will be used for all collision components
     */
    static Ref<ShaderComponent> s_shader_;

    void BuildSphereWireframe(int segments = 32);
    void BuildAABBWireframe();

public:
    CollisionComponent(WeakRef<Component> parent, float radius);
    CollisionComponent(WeakRef<Component> parent, AABB aabb);
    CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane);
    ~CollisionComponent() override;

    bool OnCreate() override;
    void OnDestroy() override;

    void Update(const float deltaTime_) override {}
    void Render() const override {}

    void RenderWireframe(const MATH::Matrix4& projMatrix,
                         const MATH::Matrix4& viewMatrix,
                         const MATH::Matrix4& modelMatrix) const;

    [[nodiscard]] Collider_type GetType() const { return type_; }
    [[nodiscard]] float GetRadius() const { return radius_; }
    [[nodiscard]] const AABB& GetAABB() const { return aabb_; }

    void SetLocalOffset(const MATH::Vec3& offset) { local_offset_ = offset; }
    [[nodiscard]] MATH::Vec3 GetLocalOffset() const { return local_offset_; }
};

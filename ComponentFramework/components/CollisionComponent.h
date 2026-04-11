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

    /**
     * The actor's uniform scale (x component is used).
     * Because the model matrix includes scale, the wireframe geometry must be
     * pre-divided by this value so it renders at the correct world-space size.
     * Must be set before OnCreate() to take effect.
     */
    float actor_scale_ = 1.0f;

    /** Set to true by the CollisionSystem when this component is involved in a collision this frame. **/
    bool is_colliding_ = false;

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

    /** Set the actor's uniform scale so the wireframe compensates for it. Must be called before OnCreate(). **/
    void SetActorScale(float scale) { actor_scale_ = scale; }
    [[nodiscard]] float GetActorScale() const { return actor_scale_; }

    void SetColliding(bool colliding) { is_colliding_ = colliding; }
    [[nodiscard]] bool IsColliding() const { return is_colliding_; }
};

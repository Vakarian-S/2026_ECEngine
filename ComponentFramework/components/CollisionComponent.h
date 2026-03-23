#pragma once
#include <cstdint>
#include <glew.h>
#include <Plane.h>
#include <Vector.h>
#include <Matrix.h>

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
    AABB aabb_;
    Collider_type type_;
    float radius_;
    MATHEX::Plane plane_;

    /** GPU wireframe geometry **/
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLsizei line_vertex_count_ = 0;

    /** Shared wireframe shader (owned by first instance, destroyed on last) **/
    static GLuint s_wire_shader_;
    static int s_instance_count_;

    /** Uniform locations inside the wireframe shader **/
    static GLint s_u_projection_;
    static GLint s_u_view_;
    static GLint s_u_model_;
    static GLint s_u_color_;

    void BuildSphereWireframe(int rings = 3, int segments = 32);
    void BuildAABBWireframe();
    static bool InitWireShader();
    static void DestroyWireShader();

public:
    CollisionComponent(WeakRef<Component> parent, float radius);
    CollisionComponent(WeakRef<Component> parent, AABB aabb);
    CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane);
    ~CollisionComponent() override;

    bool OnCreate() override;
    void OnDestroy() override;

    void Update(const float deltaTime_) override
    {
    }

    /**
     * Draws the wireframe using the provided camera matrices.
     * The model matrix should place the wireframe at the actor's world position.
     */
    void Render() const override
    {
    }

    void RenderWireframe(const MATH::Matrix4& projMatrix,
                         const MATH::Matrix4& viewMatrix,
                         const MATH::Matrix4& modelMatrix) const;

    [[nodiscard]] Collider_type GetType() const { return type_; }
    [[nodiscard]] float GetRadius() const { return radius_; }
    [[nodiscard]] const AABB& GetAABB() const { return aabb_; }
};

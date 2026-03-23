#include "CollisionComponent.h"
#include <cmath>
#include <vector>

/** Static service definitions **/
Ref<ShaderComponent> CollisionComponent::s_shader_ = nullptr;

void CollisionComponent::BuildSphereWireframe(int segments)
{
    std::vector<float> verts;

    const float pi = 3.14159265358979323846f;
    for (int plane = 0; plane < 3; ++plane)
    {
        for (int i = 0; i < segments; ++i)
        {
            float a0 = (2.0f * pi * i) / segments;
            float a1 = (2.0f * pi * (i + 1)) / segments;

            float x0 = radius_ * std::cos(a0);
            float y0 = radius_ * std::sin(a0);
            float x1 = radius_ * std::cos(a1);
            float y1 = radius_ * std::sin(a1);

            // XY plane
            if (plane == 0)
            {
                verts.insert(verts.end(), {
                    local_offset_.x + x0, local_offset_.y + y0, local_offset_.z,
                    local_offset_.x + x1, local_offset_.y + y1, local_offset_.z
                });
            }
            // XZ plane
            if (plane == 1)
            {
                verts.insert(verts.end(), {
                    local_offset_.x + x0, local_offset_.y,        local_offset_.z + y0,
                    local_offset_.x + x1, local_offset_.y,        local_offset_.z + y1
                });
            }
            // YZ plane
            if (plane == 2)
            {
                verts.insert(verts.end(), {
                    local_offset_.x,       local_offset_.y + x0, local_offset_.z + y0,
                    local_offset_.x,       local_offset_.y + x1, local_offset_.z + y1
                });
            }
        }
    }

    line_vertex_count_ = static_cast<GLsizei>(verts.size() / 3);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);
}

void CollisionComponent::BuildAABBWireframe()
{
    const MATH::Vec3& c = aabb_.center;
    const MATH::Vec3& h = aabb_.halfExtents;

    // 8 corners — offset applied so the box sits above the mesh origin
    float x0 = local_offset_.x + c.x - h.x,  x1 = local_offset_.x + c.x + h.x;
    float y0 = local_offset_.y + c.y - h.y,  y1 = local_offset_.y + c.y + h.y;
    float z0 = local_offset_.z + c.z - h.z,  z1 = local_offset_.z + c.z + h.z;

    // 12 edges, 2 verts each = 24 verts
    std::vector<float> verts = {
        // Bottom face
        x0, y0, z0, x1, y0, z0,
        x1, y0, z0, x1, y0, z1,
        x1, y0, z1, x0, y0, z1,
        x0, y0, z1, x0, y0, z0,
        // Top face
        x0, y1, z0, x1, y1, z0,
        x1, y1, z0, x1, y1, z1,
        x1, y1, z1, x0, y1, z1,
        x0, y1, z1, x0, y1, z0,
        // Vertical pillars
        x0, y0, z0, x0, y1, z0,
        x1, y0, z0, x1, y1, z0,
        x1, y0, z1, x1, y1, z1,
        x0, y0, z1, x0, y1, z1,
    };

    line_vertex_count_ = static_cast<GLsizei>(verts.size() / 3);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, float radius_)
    : Component(parent), type_(Collider_type::SPHERE), radius_(radius_)
{
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, AABB aabb_)
    : Component(parent), type_(Collider_type::AABB), aabb_(aabb_), radius_(0.0f)
{
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane_)
    : Component(parent), type_(Collider_type::PLANE), plane_(plane_), radius_(0.0f)
{
}
CollisionComponent::~CollisionComponent()
{
    if (vao_) { glDeleteVertexArrays(1, &vao_); vao_ = 0; }
    if (vbo_) { glDeleteBuffers(1, &vbo_);      vbo_ = 0; }
    line_vertex_count_ = 0;
}

bool CollisionComponent::OnCreate()
{
    /** Lazily create the shared shader the first time any CollisionComponent
     *  comes alive.  Uses ShaderComponent so we get file-based loading, the
     *  engine's error-reporting pipeline, and uniform caching for free. **/
    if (!s_shader_)
    {
        s_shader_ = std::make_shared<ShaderComponent>(
            WeakRef<Component>(),
            "shaders/wireframeVert.glsl",
            "shaders/wireframeFrag.glsl"
        );
        s_shader_->OnCreate();
    }

    if (type_ == Collider_type::SPHERE)
        BuildSphereWireframe();
    else if (type_ == Collider_type::AABB)
        BuildAABBWireframe();

    return true;
}

void CollisionComponent::OnDestroy()
{
    if (vao_)
    {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (vbo_)
    {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    line_vertex_count_ = 0;
}


void CollisionComponent::RenderWireframe(const MATH::Matrix4& proj,
                                         const MATH::Matrix4& view,
                                         const MATH::Matrix4& model) const
{
    if (vao_ == 0 || !s_shader_ || line_vertex_count_ == 0) return;

    glUseProgram(s_shader_->GetProgram());

    glUniformMatrix4fv(
        static_cast<GLint>(s_shader_->GetUniformID("projectionMatrix")),
        1, GL_FALSE, static_cast<const float*>(proj));
    glUniformMatrix4fv(
        static_cast<GLint>(s_shader_->GetUniformID("viewMatrix")),
        1, GL_FALSE, static_cast<const float*>(view));
    glUniformMatrix4fv(
        static_cast<GLint>(s_shader_->GetUniformID("modelMatrix")),
        1, GL_FALSE, static_cast<const float*>(model));

    /** Green for sphere, yellow for AABB **/
    const GLint colorLoc = static_cast<GLint>(s_shader_->GetUniformID("wireColor"));
    if (type_ == Collider_type::SPHERE)
        glUniform4f(colorLoc, 0.0f, 1.0f, 0.2f, 1.0f);
    else
        glUniform4f(colorLoc, 1.0f, 0.85f, 0.0f, 1.0f);

    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, line_vertex_count_);
    glBindVertexArray(0);

    glUseProgram(0);
}

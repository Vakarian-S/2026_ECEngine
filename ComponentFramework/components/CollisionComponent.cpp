#include "CollisionComponent.h"
#include <cmath>
#include <vector>
#include <iostream>

/** Static member definitions **/
GLuint CollisionComponent::s_wire_shader_ = 0;
int CollisionComponent::s_instance_count_ = 0;
GLint CollisionComponent::s_u_projection_ = -1;
GLint CollisionComponent::s_u_view_ = -1;
GLint CollisionComponent::s_u_model_ = -1;
GLint CollisionComponent::s_u_color_ = -1;


/** Inline GLSL sources for the wireframe shader **/
static const char* k_wireVert = R"GLSL(
#version 450
layout(location = 0) in vec3 inVertex;
layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;
void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inVertex, 1.0);
}
)GLSL";

static const char* k_wireFrag = R"GLSL(
#version 450
layout(location = 3) uniform vec4 wireColor;
layout(location = 0) out vec4 fragColor;
void main() { fragColor = wireColor; }
)GLSL";

static GLuint CompileShader(GLenum type, const char* src)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    GLint ok = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "[CollisionComponent] Shader compile error: " << log << '\n';
    }
    return id;
}

bool CollisionComponent::InitWireShader()
{
    if (s_wire_shader_ != 0) return true; // already compiled

    GLuint vert = CompileShader(GL_VERTEX_SHADER, k_wireVert);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, k_wireFrag);

    s_wire_shader_ = glCreateProgram();
    glAttachShader(s_wire_shader_, vert);
    glAttachShader(s_wire_shader_, frag);
    glLinkProgram(s_wire_shader_);

    GLint ok = 0;
    glGetProgramiv(s_wire_shader_, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(s_wire_shader_, 512, nullptr, log);
        std::cerr << "[CollisionComponent] Shader link error: " << log << '\n';
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    s_u_projection_ = glGetUniformLocation(s_wire_shader_, "projectionMatrix");
    s_u_view_ = glGetUniformLocation(s_wire_shader_, "viewMatrix");
    s_u_model_ = glGetUniformLocation(s_wire_shader_, "modelMatrix");
    s_u_color_ = glGetUniformLocation(s_wire_shader_, "wireColor");

    return true;
}

void CollisionComponent::DestroyWireShader()
{
    if (s_wire_shader_ != 0)
    {
        glDeleteProgram(s_wire_shader_);
        s_wire_shader_ = 0;
    }
}


void CollisionComponent::BuildSphereWireframe(int rings, int segments)
{
    std::vector<float> verts;

    // 3 great circles: XY, XZ, YZ planes
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

            if (plane == 0) { verts.insert(verts.end(), {x0, y0, 0, x1, y1, 0}); } // XY
            if (plane == 1) { verts.insert(verts.end(), {x0, 0, y0, x1, 0, y1}); } // XZ
            if (plane == 2) { verts.insert(verts.end(), {0, x0, y0, 0, x1, y1}); } // YZ
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

    // 8 corners
    float x0 = c.x - h.x, x1 = c.x + h.x;
    float y0 = c.y - h.y, y1 = c.y + h.y;
    float z0 = c.z - h.z, z1 = c.z + h.z;

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
    ++s_instance_count_;
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, AABB aabb_)
    : Component(parent), type_(Collider_type::AABB), aabb_(aabb_), radius_(0.0f)
{
    ++s_instance_count_;
}

CollisionComponent::CollisionComponent(WeakRef<Component> parent, MATHEX::Plane plane_)
    : Component(parent), type_(Collider_type::PLANE), plane_(plane_), radius_(0.0f)
{
    ++s_instance_count_;
}

CollisionComponent::~CollisionComponent()
{
    OnDestroy();
    --s_instance_count_;
    if (s_instance_count_ <= 0)
    {
        s_instance_count_ = 0;
        DestroyWireShader();
    }
}

bool CollisionComponent::OnCreate()
{
    InitWireShader();

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
    if (vao_ == 0 || s_wire_shader_ == 0 || line_vertex_count_ == 0) return;

    glUseProgram(s_wire_shader_);

    glUniformMatrix4fv(s_u_projection_, 1, GL_FALSE, static_cast<const float*>(proj));
    glUniformMatrix4fv(s_u_view_, 1, GL_FALSE, static_cast<const float*>(view));
    glUniformMatrix4fv(s_u_model_, 1, GL_FALSE, static_cast<const float*>(model));

    /** Green for sphere, yellow for AABB **/
    if (type_ == Collider_type::SPHERE)
        glUniform4f(s_u_color_, 0.0f, 1.0f, 0.2f, 1.0f);
    else
        glUniform4f(s_u_color_, 1.0f, 0.85f, 0.0f, 1.0f);

    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0, line_vertex_count_);
    glBindVertexArray(0);

    glUseProgram(0);
}

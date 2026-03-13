#pragma once
#include <glew.h>
#include <Vector.h>
#include <vector>

#include "Component.h"

class MeshComponent : public Component
{
private:
    const char* filename;
    std::vector<MATH::Vec3> vertices;
    std::vector<MATH::Vec3> normals;
    std::vector<MATH::Vec2> uvCoords;
    size_t dataLength;
    GLenum drawMode;
    GLuint VAO, VBO;

    /** Private helper methods **/
    void LoadModel(const char* filename_);
    void StoreMeshData(GLenum drawMode_);
    

public:
    MeshComponent(WeakRef<Component> parent_, const char* filename_);

    MeshComponent(const MeshComponent&) = delete;
    MeshComponent& operator=(const MeshComponent&) = delete;
    MeshComponent(MeshComponent&&) = delete;
    MeshComponent& operator=(MeshComponent&&) = delete;

    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Render() const override;
    void Render(GLenum drawmode_) const;
    virtual void Update(const float deltaTime_) override;
};

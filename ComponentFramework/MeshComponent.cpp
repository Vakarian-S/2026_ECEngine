#include "MeshComponent.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


void MeshComponent::LoadModel(const char* filename_)

{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, error;

    vertices.clear();
    normals.clear();
    uvCoords.clear();

    if (!LoadObj(&attrib, &shapes, &materials, &warn, &error, filename_))
    {
        throw std::runtime_error(warn + error);
    }

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            MATH::Vec3 vertex{};
            vertex.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.z = attrib.vertices[3 * index.vertex_index + 2];

            MATH::Vec3 normal{};
            normal.x = attrib.normals[3 * index.normal_index + 0];
            normal.y = attrib.normals[3 * index.normal_index + 1];
            normal.z = attrib.normals[3 * index.normal_index + 2];

            MATH::Vec2 uvCoord{};
            uvCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
            uvCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];

            vertices.push_back(vertex);
            normals.push_back(normal);
            uvCoords.push_back(uvCoord);
        }
    }
}

void MeshComponent::StoreMeshData(GLenum drawMode_)
{
    drawMode = drawMode_;
    /// These just make the code easier for me to read
#define VERTEX_LENGTH 	(vertices.size() * (sizeof(MATH::Vec3)))
#define NORMAL_LENGTH 	(normals.size() * (sizeof(MATH::Vec3)))
#define TEXCOORD_LENGTH (uvCoords.size() * (sizeof(MATH::Vec2)))

    constexpr int verticesLayoutLocation = 0;
    constexpr int normalsLayoutLocation = 1;
    constexpr int uvCoordsLayoutLocation = 2;

    /// create and bind the VOA
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    /// Create and initialize vertex buffer object VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH + TEXCOORD_LENGTH, nullptr, GL_STATIC_DRAW);

    /// assigns the addr of "points" to be the beginning of the array buffer "sizeof(points)" in length
    glBufferSubData(GL_ARRAY_BUFFER, 0, VERTEX_LENGTH, vertices.data());
    /// assigns the addr of "normals" to be "sizeof(points)" offset from the beginning and "sizeof(normals)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH, NORMAL_LENGTH, normals.data());
    /// assigns the addr of "texCoords" to be "sizeof(points) + sizeof(normals)" offset from the beginning and "sizeof(texCoords)" in length  
    glBufferSubData(GL_ARRAY_BUFFER, VERTEX_LENGTH + NORMAL_LENGTH, TEXCOORD_LENGTH, uvCoords.data());

    glEnableVertexAttribArray(verticesLayoutLocation);
    glVertexAttribPointer(verticesLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(normalsLayoutLocation);
    glVertexAttribPointer(normalsLayoutLocation, 3, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void*>(VERTEX_LENGTH));
    glEnableVertexAttribArray(uvCoordsLayoutLocation);
    glVertexAttribPointer(uvCoordsLayoutLocation, 2, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void*>(VERTEX_LENGTH + NORMAL_LENGTH));

    dataLength = vertices.size();

    /// give back the memory used in these vectors. The data is safely stored in the GPU now
    vertices.clear();
    normals.clear();
    uvCoords.clear();

    /// Don't need these defines sticking around anymore si undefine them. 
#undef VERTEX_LENGTH
#undef NORMAL_LENGTH
#undef TEXCOORD_LENGTH
}

MeshComponent::MeshComponent(WeakRef<Component> parent_, const char* filename_) :
    Component(parent_), dataLength(0), drawMode(GL_TRIANGLES), VAO(0), VBO(0), filename(filename_)
{
}

bool MeshComponent::OnCreate()
{
    LoadModel(filename);
    StoreMeshData(drawMode);
    return true;
}


void MeshComponent::Render() const
{
    glBindVertexArray(VAO);
    glDrawArrays(drawMode, 0, static_cast<GLsizei>(dataLength));
    glBindVertexArray(0); // Unbind the VAO
}

void MeshComponent::Render(GLenum drawmode_) const
{
    glBindVertexArray(VAO);
    glDrawArrays(drawmode_, 0, static_cast<GLsizei>(dataLength));
    glBindVertexArray(0); // Unbind the VAO
}

void MeshComponent::OnDestroy()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void MeshComponent::Update(const float deltaTime_)
{
}

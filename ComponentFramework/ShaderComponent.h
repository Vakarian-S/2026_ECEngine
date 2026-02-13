#pragma once
#include <glew.h>
#include <string>
#include <unordered_map>

#include "Component.h"

class ShaderComponent : public Component
{
private:
    const char* vertexShaderFilename;
    const char* fragmentShaderFilename;
    const char* tessellationControllerFilename;
    const char* tesselationEvaluatorFilename;
    const char* geometryShaderFilename;

    GLuint shaderID;
    GLuint vertShaderID;
    GLuint fragShaderID;
    GLuint tessCtrlShaderID;
    GLuint tessEvalShaderID;
    GLuint geomShaderID;
    std::unordered_map<std::string, GLuint> uniformMap;

    /// Private helper methods
    char* ReadTextFile(const char* fileName);
    bool CompileAttach();
    bool Link();
    void SetUniformLocations();

public:
    ShaderComponent(Component* parent_, const char* vertexShaderFilename_, const char* fragmentShaderFilename_,
                    const char* tesselationControllerFilename = nullptr,
                    const char* tesselationEvaluatorFilename = nullptr,
                    const char* geometryShaderFilename = nullptr);
    ~ShaderComponent() override;

    ShaderComponent(const ShaderComponent&) = delete;
    ShaderComponent(ShaderComponent&&) = delete;
    ShaderComponent& operator =(const ShaderComponent&) = delete;
    ShaderComponent& operator =(ShaderComponent&&) = delete;

    bool OnCreate() override;
    void OnDestroy() override;
    void Update(const float deltaTime) override;
    void Render() const override;

    GLuint GetUniformID(std::string name);
    inline GLuint GetProgram() const { return shaderID; }
};

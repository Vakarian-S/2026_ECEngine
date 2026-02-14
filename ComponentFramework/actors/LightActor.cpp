#include "LightActor.h"

#include <glew.h>

#include "../ShaderComponent.h"
#include "../TransformComponent.h"

LightActor::LightActor(Component* parentComponent)
    : Actor(parentComponent)
{
}

bool LightActor::OnCreate()
{
    TransformComponent* transformComponent = GetComponent<TransformComponent>();
    if (transformComponent == nullptr)
    {
        TransformComponent* newTransform = new TransformComponent(nullptr, Vec3(0.0f, 0.0f, 0.0f),
                                                                  Quaternion(),
                                                                  Vec3(0.5f, 0.5f, 0.5f));

        AddComponent<TransformComponent>(newTransform);
    }

    return Actor::OnCreate();
}

void LightActor::Update(const float deltaTime)
{
}


void LightActor::ApplyPointLightUniformsToShaderProgram(ShaderComponent* shader) const
{
    glUniform3fv(static_cast<GLint>(shader->GetUniformID("lightPos")), 1,
                 GetComponent<TransformComponent>()->GetPosition());
    glUniform4fv(static_cast<GLuint>(shader->GetUniformID("diffuseMaterialColor")), 1, pointLightParameters.diffuseLightColor);
    glUniform4fv(static_cast<GLuint>(shader->GetUniformID("specularMaterialColor")), 1, pointLightParameters.specularLightColor);
    glUniform1f(static_cast<GLuint>(shader->GetUniformID("specularShininessExponent")), pointLightParameters.specularShininessExponent);
    glUniform1f(static_cast<GLuint>(shader->GetUniformID("lightIntensityMultiplier")), pointLightParameters.lightIntensityMultiplier);
    glUniform1f(static_cast<GLuint>(shader->GetUniformID("attenuationConstant")), pointLightParameters.attenuationConstant);
    glUniform1f(static_cast<GLuint>(shader->GetUniformID("attenuationLinear")), pointLightParameters.attenuationLinear);
    glUniform1f(static_cast<GLuint>(shader->GetUniformID("attenuationQuadratic")), pointLightParameters.attenuationQuadratic);
}

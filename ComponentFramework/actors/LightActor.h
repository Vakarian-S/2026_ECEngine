#pragma once
#include "../Actor.h"
#include "../ShaderComponent.h"


struct PointLightParameters
{
    MATH::Vec3 lightWorldPosition = MATH::Vec3(0.0f, 0.0f, 0.0f);

    /** Phong Parameters **/
    MATH::Vec3 diffuseLightColor = MATH::Vec3(1.0f, 1.0f, 1.0f);
    MATH::Vec3 specularLightColor = MATH::Vec3(1.0f, 1.0f, 1.0f);
    float specularShininessExponent = 14.0f;
    float lightIntensityMultiplier = 1.0f;

    /** Attenuation Values **/
    float attenuationConstant = 1.0f;
    float attenuationLinear = 0.09f;
    float attenuationQuadratic = 0.032f;
};

class LightActor : public Actor
{
private:
    PointLightParameters pointLightParameters;

public:
    LightActor(Component* parentComponent);
    bool OnCreate() override;
    void Update(const float deltaTime) override;

    // Light parameter accessors (small and explicit)
    void SetDiffuseLightColor(const MATH::Vec3& diffuseLightColor)
    {
        pointLightParameters.diffuseLightColor = diffuseLightColor;
    }

    void SetSpecularLightColor(const MATH::Vec3& specularLightColor)
    {
        pointLightParameters.specularLightColor = specularLightColor;
    }

    void SetLightIntensityMultiplier(const float lightIntensityMultiplier)
    {
        pointLightParameters.lightIntensityMultiplier = lightIntensityMultiplier;
    }

    void SetAttenuationParameters(
        const float attenuationConstant,
        const float attenuationLinear,
        const float attenuationQuadratic
    )
    {
        pointLightParameters.attenuationConstant = attenuationConstant;
        pointLightParameters.attenuationLinear = attenuationLinear;
        pointLightParameters.attenuationQuadratic = attenuationQuadratic;
    }

    [[nodiscard]] const PointLightParameters& GetPointLightParameters() const
    {
        return pointLightParameters;
    }

    void ApplyPointLightUniformsToShaderProgram(ShaderComponent* shader) const;
};

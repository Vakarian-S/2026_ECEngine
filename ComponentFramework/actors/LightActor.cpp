#include "LightActor.h"

#include <glew.h>

#include "../ShaderComponent.h"
#include "../TransformComponent.h"

LightActor::LightActor(Ref<Component> parentComponent)
    : Actor(parentComponent)
{
}

bool LightActor::OnCreate()
{
    Ref transformComponent = GetComponent<TransformComponent>();
    if (transformComponent == nullptr)
    {
        auto newTransform = std::make_shared<TransformComponent>(nullptr, Vec3(0.0f, 0.0f, 0.0f),
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
}

#include "Actor.h"

#include <Matrix.h>

#include "Debug.h"
#include "TransformComponent.h"

class TransformComponent;

Actor::Actor(WeakRef<Component> parent_) : Component(parent_)
{
}

bool Actor::OnCreate()
{
    if (isCreated) return true;
    Debug::Info("Loading assets for Actor: ", __FILE__, __LINE__);
    for (auto component : components)
    {
        if (component.get()->OnCreate() == false)
        {
            Debug::Error("Loading assets for Actor/Components: ", __FILE__, __LINE__);
            isCreated = false;
            return isCreated;
        }
    }
    isCreated = true;
    return isCreated;
}

Actor::~Actor()
{
    OnDestroy();
}

void Actor::OnDestroy()
{
    Debug::Info("Deleting assets for Actor: ", __FILE__, __LINE__);
    RemoveAllComponents();
    isCreated = false;
}


void Actor::Update(const float deltaTime)
{
    std::cout << "Hello from Update\n";
}

void Actor::Render() const
{
}

void Actor::RemoveAllComponents()
{
    components.clear();
}

void Actor::ListComponents() const
{
    std::cout << typeid(*this).name() << " contains the following components:\n";
    for (auto component : components)
    {
        std::cout << typeid(*component).name() << std::endl;
    }
    std::cout << '\n';
}

Matrix4 Actor::GetModelMatrix()
{
    if (const Ref transform = GetComponent<TransformComponent>())
    {
        modelMatrix = transform->GetTransformMatrix();
    }
    else
    {
        modelMatrix.loadIdentity();
    }
    if (const auto parentPtr = parent.lock())
    {
        modelMatrix = dynamic_cast<Actor*>(parentPtr.get())->GetModelMatrix() * modelMatrix;
    }
    return modelMatrix;
}

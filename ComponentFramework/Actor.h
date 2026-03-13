#pragma once
#include <vector>
#include <iostream>
#include <Matrix.h>

#include "Component.h"

class Actor : public Component
{
    Actor(const Actor&) = delete;
    Actor(Actor&&) = delete;
    Actor& operator=(const Actor&) = delete;
    Actor& operator=(Actor&&) = delete;

    MATH::Matrix4 modelMatrix;

protected:
    std::vector<Ref<Component>> components;

public:
    Actor(WeakRef<Component> parent_);
    ~Actor();
    virtual bool OnCreate() override;
    virtual void OnDestroy() override;
    virtual void Update(const float deltaTime) override;
    virtual void Render() const override;

    template <typename ComponentTemplate>
    void AddComponent(Ref<ComponentTemplate> component_)
    {
        if (GetComponent<ComponentTemplate>().get() != nullptr)
        {
#ifdef _DEBUG
            std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
            return;
        }
        components.push_back(component_);
    }


    template <typename ComponentTemplate, typename... Args>
    void AddComponent(Args&&... args_)
    {
        if (GetComponent<ComponentTemplate>().get() != nullptr)
        {
#ifdef _DEBUG
            std::cerr << "WARNING: Trying to add a component type that is already added - ignored\n";
#endif
            return;
        }
        components.push_back(std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...));
    }

    template <typename ComponentTemplate>
    [[nodiscard]] Ref<ComponentTemplate> GetComponent() const
    {
        for (auto component : components)
        {
            if (dynamic_cast<ComponentTemplate*>(component.get()))
            {
                /// This is a dynamic cast designed for shared_ptr's
                /// https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
                return std::dynamic_pointer_cast<ComponentTemplate>(component);
            }
        }
        return Ref<ComponentTemplate>(nullptr);
    }

    template <typename ComponentTemplate>
    void RemoveAllComponents()
    {
        components.clear();
    }


    void ListComponents() const;
    MATH::Matrix4 GetModelMatrix();
    void RemoveAllComponents();
};

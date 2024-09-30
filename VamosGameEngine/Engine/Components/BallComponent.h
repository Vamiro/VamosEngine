#pragma once
#include <iostream>

#include "ColliderComponent.h"
#include "Component.h"
#include "Engine/Core/GameObject.h"

class BallComponent : public Component
{
public:
    BallComponent(GameObject& parent)
        : Component(parent, "BallComponent")
    {
    }

    void Start() override;
    void Update() override;
    void RenderGUI() override;

    void Destroy() override
    {
    }

private:
    ColliderComponent* colliderComponent = nullptr;
};

inline void BallComponent::Start()
{
    colliderComponent = parent->GetComponent<ColliderComponent>();
    auto collider = parent->GetComponent<ColliderComponent>();
    if (collider)
    {
        colliderComponent->OnCollision().BindLambda([this](ColliderComponent* other) {
            std::cout << "Collision with " << other->GetParent().name << std::endl;
            if(other->GetBody() != nullptr && other->GetBody()->IsDynamic())
            {
                other->GetParent().SetParent(this->parent);
                other->Destroy();
                other->GetParent().DeleteComponent(other);
            }
        });
    }
}

inline void BallComponent::Update()
{
}

inline void BallComponent::RenderGUI()
{
}

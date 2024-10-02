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
    void Update(float deltaTime) override;
    void RenderGUI() override;

    void Destroy() override
    {
    }

private:
    ColliderComponent* colliderComponent = nullptr;
    ColliderComponent* otherCollider = nullptr;
    int items = 0;
};

inline void BallComponent::Start()
{
    colliderComponent = parent->GetComponent<ColliderComponent>();
    auto collider = parent->GetComponent<ColliderComponent>();
    if (collider)
    {
        colliderComponent->OnCollision().BindLambda([this](ColliderComponent* other) {
            std::cout << "Collision with " << other->GetParent().name << std::endl;
            if(other->GetParent().name != "Floor" && other->GetParent().name != "Ball")
            {
                otherCollider = other;
            }
        });
    }
}

inline void BallComponent::Update(float deltaTime)
{
    if (parent->GetChildren().size() != items)
    {
        items = parent->GetChildren().size();
        auto size = items / 50.0f + 1.0f;
        colliderComponent->SetScale(size);
        //parent->transform->SetScale({size,size,size});
    }
    if (otherCollider != nullptr)
    {
        otherCollider->GetParent().SetParent(this->parent);
        otherCollider->SetLayer(Layers::PLAYER);
        otherCollider->SetMotionType(JPH::EMotionType::Kinematic);
        otherCollider = nullptr;
    }
}

inline void BallComponent::RenderGUI()
{
}

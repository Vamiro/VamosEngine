#pragma once
#include "Transform.h"

class FollowCamera;
class ColliderComponent;

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
    FollowCamera* followCamera = nullptr;
    int items = 0;
    float ballSpeed = 1.0f;
};
#pragma once
#include "Transform.h"

class FollowCamera : public Component
{
public:
    FollowCamera(GameObject& parent, Transform& cameraTransform);
    ~FollowCamera() override = default;

    void Start() override;
    void Update(float deltaTime) override;
    void RotateAround(float mouseX);
    void Follow();
    void RenderGUI() override;
    void Destroy() override;

    Transform* cameraTransform;

private:
    GameObject* target = nullptr;

    float distance = 7.0f;
    float height = 3.0f;
    DirectX::SimpleMath::Vector3 centerOffset = DirectX::SimpleMath::Vector3::Zero;
    bool followOnStart = false;


    // Cache for camera offset
    DirectX::SimpleMath::Vector3 cameraOffset = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 initialCameraOffset = DirectX::SimpleMath::Vector3::Zero;
};

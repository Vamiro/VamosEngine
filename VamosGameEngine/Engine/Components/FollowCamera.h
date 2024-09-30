#pragma once
#include "Component.h"

class FollowCamera : public Component
{
public:
    FollowCamera(GameObject& parent, GameObject& target);
    ~FollowCamera() override = default;

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void Destroy() override;

private:
    GameObject* target = nullptr;
};

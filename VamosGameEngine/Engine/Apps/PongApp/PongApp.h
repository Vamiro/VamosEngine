#pragma once

#include "Engine/Core/GameEngine.h"

class PongApp : public GameEngine{
public:
    bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                    int height) override;
    void Update() override;
    void RenderGui() override;
    void AddComponentToObject(GameObject* obj, const std::string& component_name);
    bool InitializeScene() override;

private:
    float maxSpeed = 0.02f;
    SimpleMath::Vector3 speed = SimpleMath::Vector3(0.015f, 0.0f, 0.015f);
    SimpleMath::Vector3 currentSpeed = speed;

    GameObject* ball;
    GameObject* floor;
};


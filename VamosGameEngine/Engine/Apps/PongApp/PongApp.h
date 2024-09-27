#pragma once

#include <DirectXColors.h>

#include "Engine/Core/GameEngine.h"
#include "GameObjects/BoxObject.h"
#include "GameObjects/SphereObject.h"

class PongApp : public GameEngine{
public:
    bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                    int height) override;
    void Update() override;
    void ResetBall();
    void RenderGui() override;
    void AddComponentToObject(Object* obj, const std::string& component_name);
    bool InitializeScene() override;

private:
    float maxSpeed = 0.02f;
    SimpleMath::Vector3 speed = SimpleMath::Vector3(0.015f, 0.0f, 0.015f);
    SimpleMath::Vector3 currentSpeed = speed;

    int scoreLeft = 0;
    int scoreRight = 0;

    GameObject* ball;

    // BoxObject* wallTop;
    // BoxObject* wallBottom;
    // BoxObject* wallLeft;
    // BoxObject* wallRight;
};


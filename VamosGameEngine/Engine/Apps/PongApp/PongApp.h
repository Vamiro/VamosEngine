#pragma once
#include "Engine/Core/GameEngine.h"
#include "GameObjects/BoxObject.h"
#include "GameObjects/SphereObject.h"

class PongApp : public GameEngine{
public:
    bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                    int height) override;
    void Update() override;
    void ResetBall();
    void RenderGui() override;
    bool InitializeScene() override;

private:
    SimpleMath::Vector3 speed = SimpleMath::Vector3(0.015f, 0.0f, 0.015f);
    SimpleMath::Vector3 currentSpeed = speed;
    SphereObject* ball;

    BoxObject* playerLeft;
    BoxObject* playerRight;

    int scoreLeft = 0;
    int scoreRight = 0;

    BoxObject* wallTop;
    BoxObject* wallBottom;
    BoxObject* wallLeft;
    BoxObject* wallRight;
};


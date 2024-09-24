#pragma once
#include "Engine/Core/GameEngine.h"
#include "GameObjects/BoxObject.h"
#include "GameObjects/SphereObject.h"

class PongApp : public GameEngine{
public:
    bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                    int height) override;
    void Update() override;
    void RenderGui() override;
    bool InitializeScene() override;

private:
    float Speed = 0.01f;
    float currentSpeed = Speed;
    SphereObject* ball;

    BoxObject* playerLeft;
    BoxObject* playerRight;

    BoxObject* wallTop;
    BoxObject* wallBottom;
    BoxObject* wallLeft;
    BoxObject* wallRight;
};


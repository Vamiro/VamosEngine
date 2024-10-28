#pragma once
#include <string>
#include "Engine.h"
#include "Engine/Graphics/ConstantBuffer.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Core/GameObject.h"

class GameEngine : public Engine {
public:
    GameEngine();
    ~GameEngine() override;
    bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) override;
    bool ProcessMessages() override;
    void Update() override;
    void UpdatePhysics();
    void RenderFrame() override;
    void RenderGui() override;
    bool InitializeScene() override;

protected:
    Timer timer;
    float deltaTime = 0;

    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    std::vector<GameObject*> gameObjects;
    int currentGameObj = 0;

    DirectX::SimpleMath::Vector3 _lightDirection;
    void InitializePhysics();
};

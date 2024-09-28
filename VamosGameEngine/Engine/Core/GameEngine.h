#pragma once
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "GameObject.h"
#include "Engine.h"
#include "Engine/Graphics/ConstantBuffer.h"
#include "Engine/Physics/PhysicsEngine.h"
#include "Engine/Utilities/Timer.h"


class GameEngine : public Engine {
public:
    GameEngine() = default;
    ~GameEngine() override;
    bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) override;
    bool ProcessMessages() override;
    void Update() override;
    void UpdatePhysics(float deltaTime);
    void RenderFrame() override;
    void RenderGui() override;
    bool InitializeScene() override;

    void SetCurrentCamera(Camera* camera) { currentCamera = camera; }

protected:
    Timer timer;
    float deltaTime = 0;

    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    Camera* currentCamera;
    std::vector<GameObject*> gameObjects;

    int currentGameObj = 0;

    PhysicsEngine* physicsEngine;
    void InitializePhysics();
};

#pragma once
#include <string>
#include "Engine.h"
#include "Engine/Graphics/ConstantBuffer.h"
#include "Engine/Physics/PhysicsEngine.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Rendering/Camera.h"

class GameEngine : public Engine {
public:
    GameEngine();
    ~GameEngine() override;
    bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) override;
    bool ProcessMessages() override;
    void Update() override;
    void UpdatePhysics(float deltaTime);
    void RenderFrame() override;
    void RenderGui() override;
    bool InitializeScene() override;

    [[nodiscard]] static InputDevice& Input() { return *input_device_; }
    [[nodiscard]] static Graphics& GetGraphics() { return *gfx_; }
    [[nodiscard]] static JPH::BodyInterface& GetBodyInterface() { return *_bodyInterface; }

    [[nodiscard]] static Camera* GetCurrentCamera() { return currentCamera; }
    static void SetCurrentCamera(Camera* camera) { currentCamera = camera; }

protected:
    Timer timer;
    float deltaTime = 0;

    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    static Camera* currentCamera;
    std::vector<GameObject*> gameObjects;
    int currentGameObj = 0;

    PhysicsEngine* physicsEngine;
    static JPH::BodyInterface* _bodyInterface;
    DirectX::SimpleMath::Vector3 _lightDirection;
    void InitializePhysics();
};

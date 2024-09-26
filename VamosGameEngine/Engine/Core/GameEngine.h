#pragma once
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include "Engine/Physics/PhysicsLayers.h"

#include "GameObject.h"
#include "Engine.h"
#include "Engine/Graphics/ConstantBuffer.h"
#include "Engine/Utilities/Timer.h"


class GameEngine : public Engine {
public:
    bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) override;
    bool ProcessMessages() override;
    void Update() override;
    void UpdatePhysics(float deltaTime);
    void RenderFrame() override;

protected:
    Timer timer;
    float deltaTime = 0;

    JPH::PhysicsSystem physicsSystem;
    JPH::TempAllocatorImpl* tempAllocator = nullptr;
    JPH::JobSystemThreadPool* jobSystem = nullptr;

    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    std::vector<GameObject*> gameObjects;
    int currentGameObj = 0;

    MyBroadPhaseLayer broadPhaseLayer;
    MyObjectVsBroadPhaseLayerFilter objectVsBroadPhaseLayerFilter;
    MyObjectLayerPairFilter objectLayerPairFilter;
    void InitializePhysics();
};

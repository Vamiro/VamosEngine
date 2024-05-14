#include "GameEngine.h"

#include <iostream>

bool GameEngine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                            int height)
{
    timer.Start();
    if (!this->window_.Initialize(this, hInstance, window_title, window_class, width, height))
    {
        return false;
    }
    if (!this->gfx_.Initialize(window_.GetHWND(), width, height))
    {
        return false;
    }
    
    input_device_.OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
        gfx_.camera.AdjustRotation(args.Offset.y * input_device_.MouseSensitivity, args.Offset.x * input_device_.MouseSensitivity, 0.0f);
    });
    
    return true;
}

bool GameEngine::ProcessMessages()
{
    return this->window_.ProcessMessages();
}

void GameEngine::Update()
{
    float dt = timer.GetMilisecondsElapsed();
    timer.Restart();
    if (this->gfx_.blockInputForImGui)
    {
        return;
    }

    if (input_device_.IsKeyDown(InputKey::LeftShift) || input_device_.IsKeyDown(InputKey::RightShift))
    {
        return;
    }
    
    constexpr float cameraSpeed = 0.002f;
    constexpr float rotationSpeed = 0.002f;

    if (input_device_.IsKeyDown(InputKey::W))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetForwardVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::S))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetBackwardVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::A))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetLeftVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::D))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetRightVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::Q))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetUpVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::E))
    {
        gfx_.camera.AdjustPosition(gfx_.camera.GetUpVector() * cameraSpeed * dt * -1.0f);
    }
    
    gfx_.gameObject.AdjustRotation(0.0f, rotationSpeed * dt, 0.0f);
}

void GameEngine::RenderFrame()
{
    gfx_.RenderFrame();
}

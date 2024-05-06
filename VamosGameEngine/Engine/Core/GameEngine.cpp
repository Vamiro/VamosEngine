#include "GameEngine.h"

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

    float mouseSensitivity = 0.01f;
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

    gfx_.gameObject.AdjustRotation(0.0f, rotationSpeed * dt, 0.0f);

    if (input_device_.IsMouseMoved())
    {
        SimpleMath::Vector2 mousePosition = input_device_.MousePosition;
        gfx_.camera.AdjustRotation(0.0f, -mousePosition.y * mouseSensitivity, mousePosition.x * mouseSensitivity);
    }
}

void GameEngine::RenderFrame()
{
    gfx_.RenderFrame();
}

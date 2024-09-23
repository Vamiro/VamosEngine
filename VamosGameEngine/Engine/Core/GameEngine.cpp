#include "GameEngine.h"

#include <iostream>

bool GameEngine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                            int height)
{
    timer.Start();
    if (!this->window_.Initialize(this, hInstance, window_title, window_class, width, height))
        return false;

    if (!this->gfx_.Initialize(window_.GetHWND(), width, height))
        return false;

    gameObject = new GameObject();

    if (!this->InitializeScene())
        return false;

    input_device_.OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
        if(input_device_.IsKeyDown(InputKey::RightButton))
            gfx_.camera.transform.AdjustRotation(
                args.Offset.y * input_device_.MouseSensitivity,
                args.Offset.x * input_device_.MouseSensitivity,
                0.0f);
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

    // Установка шейдера
    gfx_.shaderManager->SetShader(ShaderData("Data\\Shaders\\simpleShader.hlsl", PixelType | VertexType));

    gfx_.camera.UpdateViewMatrix();
    constexpr float cameraSpeed = 0.002f;
    constexpr float rotationSpeed = 0.002f;

    if (input_device_.IsKeyDown(InputKey::W))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetForwardVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::S))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetBackwardVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::A))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetLeftVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::D))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetRightVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::Q))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetUpVector() * cameraSpeed * dt);
    }
    if (input_device_.IsKeyDown(InputKey::E))
    {
        gfx_.camera.transform.AdjustPosition(gfx_.camera.transform.GetUpVector() * cameraSpeed * dt * -1.0f);
    }
    
    gameObject->transform.AdjustRotation(0.0f, rotationSpeed * dt, 0.0f);
}

void GameEngine::RenderFrame()
{
    gfx_.RenderFrame();
}

void GameEngine::RenderGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("GUI");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
    ImGui::ColorEdit4("Object Color", reinterpret_cast<float*>(&objectColor));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Устанавливаем цвет объекта в зависимости от значения слайдера
    gameObject->SetColor(objectColor);
}

bool GameEngine::InitializeScene()
{
    try
    {
        Microsoft::WRL::ComPtr<ID3D11Device> d3d_device = gfx_.GetDevice();
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_device_context = gfx_.GetDeviceContext();

        HRESULT hr = cb_vs_vertexshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create vertexshader constant buffer.");
        hr = cb_ps_pixelshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create pixelshader constant buffer.");

        gameObject->Initialize("Data\\Objects\\sphere.obj", d3d_device.Get(),
            d3d_device_context.Get(), this->cb_vs_vertexshader, this->cb_ps_pixelshader);
        gameObject->transform.SetPosition(-2.0f, 0.0f, 4.0f);

        gfx_.gameObjects.emplace_back(gameObject);

        gameObject->Initialize("Data\\Objects\\sphere.obj", d3d_device.Get(),
            d3d_device_context.Get(), this->cb_vs_vertexshader, this->cb_ps_pixelshader);

        gameObject->transform.SetPosition(2.0f, 0.0f, 4.0f);

        gfx_.gameObjects.emplace_back(gameObject);

        gfx_.camera.transform.SetPosition(0.0f, 0.0f, 0.0f);
        gfx_.camera.SetProjectionValues(90.0f, static_cast<float>(gfx_.GetWindowWidth()) / static_cast<float>(gfx_.GetWindowHeight()), 0.1f,
                                   1000.0f);
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}

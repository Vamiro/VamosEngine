﻿//
// Created by Vovka on 24.09.2024.
//

#include "PongApp.h"

#include "GameObjects/SphereObject.h"

bool PongApp::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    GameEngine::Initialize(hInstance, window_title, window_class, width, height);

    input_device_.OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
        if(input_device_.IsKeyDown(InputKey::RightButton))
            gfx_.camera.transform.AdjustRotation(
                args.Offset.y * input_device_.MouseSensitivity,
                args.Offset.x * input_device_.MouseSensitivity,
                0.0f);
    });

    return true;
}

void PongApp::Update()
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

}

void PongApp::RenderGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("GUI");
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

    auto gameObject_getter = [](void* data, int index, const char** output) -> bool
    {
        auto* gameObjects = static_cast<std::vector<GameObject*>*>(data);
        if (index < 0 || index >= static_cast<int>(gameObjects->size()))
            return false;

        GameObject* current_gameObject = (*gameObjects)[index];
        if (!current_gameObject)
            return false;

        *output = current_gameObject->name.c_str();
        return true;
    };

    ImGui::ListBox(
        "Objects",
        &currentGameObj,
        gameObject_getter,
        &gameObjects,
        gameObjects.size());

    SimpleMath::Color currentColor = gameObjects[currentGameObj]->GetColor();
    XMFLOAT3 currentPos = gameObjects[currentGameObj]->transform.GetPositionFloat3();
    float pos[3] = {currentPos.x, currentPos.y, currentPos.z};

    ImGui::ColorEdit4("Object Color", reinterpret_cast<float*>(&currentColor));
    ImGui::DragFloat3("Position", pos, 0.1f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Устанавливаем цвет объекта в зависимости от значения слайдера
    gameObjects[currentGameObj]->SetColor(currentColor);
    gameObjects[currentGameObj]->transform.SetPosition(XMFLOAT3(pos));
}

bool PongApp::InitializeScene()
{
    try
    {
        Microsoft::WRL::ComPtr<ID3D11Device> d3d_device = gfx_.GetDevice();
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3d_device_context = gfx_.GetDeviceContext();

        HRESULT hr = cb_vs_vertexshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create vertexshader constant buffer.");
        hr = cb_ps_pixelshader.Initialize(d3d_device.Get(), d3d_device_context.Get());
        ErrorLogger::Log(hr, "Failed to create pixelshader constant buffer.");

        auto* so = new SphereObject("Sphere1");
        so->Initialize("Data\\Objects\\sphere.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        so->transform.SetPosition(XMFLOAT3(0.0f, 0.0f, 4.0f));

        gameObjects.emplace_back(so);
        gfx_.gameObjects = gameObjects;

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

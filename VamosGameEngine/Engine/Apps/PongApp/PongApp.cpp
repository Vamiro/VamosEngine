#include "PongApp.h"

bool PongApp::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
    GameEngine::Initialize(hInstance, window_title, window_class, width, height);

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
    gfx_.shaderManager->SetShader(ShaderData("Data\\Shaders\\simpleShader.hlsl", PixelType | VertexType));

    if (input_device_.IsKeyDown(InputKey::W)) playerRight->transform.AdjustPosition(0, 0, 0.01f * dt);
    if (input_device_.IsKeyDown(InputKey::S)) playerRight->transform.AdjustPosition(0, 0, -0.01f * dt);
    if (input_device_.IsKeyDown(InputKey::Up)) playerLeft->transform.AdjustPosition(0, 0, 0.01f * dt);
    if (input_device_.IsKeyDown(InputKey::Down)) playerLeft->transform.AdjustPosition(0, 0, -0.01f * dt);

    ball->transform.AdjustPosition(currentSpeed * dt, 0.0f, 0.0f);

    ball->SetBoundingCenter(ball->transform.GetPositionFloat3());
    playerLeft->SetBoundingCenter(playerLeft->transform.GetPositionFloat3());
    playerRight->SetBoundingCenter(playerRight->transform.GetPositionFloat3());

    if (ball->GetBoundingSphere().Intersects(playerLeft->GetBoundingBox()))
    {
        currentSpeed = Speed * -1;
    }

    if (ball->GetBoundingSphere().Intersects(playerRight->GetBoundingBox()))
    {
        currentSpeed = Speed;
    }

    gfx_.camera.UpdateViewMatrix();

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

    XMFLOAT3 currentScale = gameObjects[currentGameObj]->transform.GetScaleFloat3();
    float scale[3] = {currentScale.x, currentScale.y, currentScale.z};

    ImGui::ColorEdit4("Object Color", reinterpret_cast<float*>(&currentColor));
    ImGui::DragFloat3("Position", pos, 0.1f);
    ImGui::DragFloat3("Scale", scale, 0.1f);
    ImGui::SliderFloat("Speed", &Speed, 0.01f, 1.0f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Устанавливаем цвет объекта в зависимости от значения слайдера
    gameObjects[currentGameObj]->SetColor(currentColor);
    gameObjects[currentGameObj]->transform.SetPosition(XMFLOAT3(pos));
    gameObjects[currentGameObj]->transform.SetScale(XMFLOAT3(scale));
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


        ball = new SphereObject("Ball");
        ball->Initialize("Data\\Objects\\sphere.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        ball->transform.SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
        ball->SetBoundingSphereRadius(1.f);
        gameObjects.emplace_back(ball);

        playerLeft = new BoxObject("PlayerRight");
        playerLeft->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        playerLeft->transform.SetPosition(XMFLOAT3(12.0f, 0.0f, 0.0f));
        playerLeft->transform.SetScale(XMFLOAT3(0.2f, 2.0f, 2.0f));
        playerLeft->SetBoundingBoxExtention(SimpleMath::Vector3(0.2f, 2.0f, 2.0f));
        gameObjects.emplace_back(playerLeft);

        playerRight = new BoxObject("PlayerLeft");
        playerRight->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        playerRight->transform.SetPosition(XMFLOAT3(-12.0f, 0.0f, 0.0f));
        playerRight->transform.SetScale(XMFLOAT3(0.2f, 2.0f, 2.0f));
        playerRight->SetBoundingBoxExtention(SimpleMath::Vector3(0.2f, 2.0f, 2.0f));
        gameObjects.emplace_back(playerRight);

        wallTop = new BoxObject("wallTop");
        wallTop->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        wallTop->transform.SetPosition(XMFLOAT3(0.0f, 0.0f, 8.0f));
        wallTop->transform.SetScale(XMFLOAT3(14.0f, 2.0f, 0.2f));
        wallTop->SetBoundingBoxExtention(SimpleMath::Vector3(14.0f, 2.0f, 0.2f));
        gameObjects.emplace_back(wallTop);

        wallBottom = new BoxObject("wallBottom");
        wallBottom->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        wallBottom->transform.SetPosition(XMFLOAT3(0.0f, 0.0f, -8.0f));
        wallBottom->transform.SetScale(XMFLOAT3(14.0f, 2.0f, 0.2f));
        wallBottom->SetBoundingBoxExtention(SimpleMath::Vector3(14.0f, 2.0f, 0.2f));
        gameObjects.emplace_back(wallBottom);

        wallLeft = new BoxObject("wallLeft");
        wallLeft->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        wallLeft->transform.SetPosition(XMFLOAT3(-13.8f, -1.0f, 0.0f));
        wallLeft->transform.SetScale(XMFLOAT3(0.2f, 2.0f, 8.0f));
        wallLeft->SetBoundingBoxExtention(SimpleMath::Vector3(0.2f, 2.0f, 8.0f));
        wallLeft->SetColor(SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
        gameObjects.emplace_back(wallLeft);

        wallRight = new BoxObject("wallRight");
        wallRight->Initialize("Data\\Objects\\box.obj", d3d_device.Get(), d3d_device_context.Get(), cb_vs_vertexshader, cb_ps_pixelshader);
        wallRight->transform.SetPosition(XMFLOAT3(13.8f, -1.0f, 0.0f));
        wallRight->transform.SetScale(XMFLOAT3(0.2f, 2.0f, 8.0f));
        wallRight->SetBoundingBoxExtention(SimpleMath::Vector3(0.2f, 2.0f, 8.0f));
        wallRight->SetColor(SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
        gameObjects.emplace_back(wallRight);

        gfx_.gameObjects = gameObjects;

        gfx_.camera.transform.SetPosition(0.0f, 100.0f, 0.0f);
        gfx_.camera.transform.SetRotation(Transform::DegreesToRadians(90.0f, 0.0f, 0.0f));

        std::cout << gfx_.camera.transform.GetRotationFloat3().x;
        gfx_.camera.SetProjectionValues(30.0f, static_cast<float>(gfx_.GetWindowWidth()) / static_cast<float>(gfx_.GetWindowHeight()), 0.1f,
                                   1000.0f, ORTHOGRAPHIC);
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}

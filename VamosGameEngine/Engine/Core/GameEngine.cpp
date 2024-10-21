#include "GameEngine.h"


JPH::BodyInterface* GameEngine::_bodyInterface = nullptr;

GameEngine::GameEngine(): physicsEngine(nullptr)
{
    _lightDirection = DirectX::SimpleMath::Vector3(0.0f, -1.0f, 1.0f);
}

GameEngine::~GameEngine()
{
    for (const auto gameObject : gameObjects)
    {
        gameObject->Destroy();
        delete gameObject;
    }
}

bool GameEngine::Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width,
                            int height)
{
    timer.Start();
    if (!this->window_.Initialize(this, hInstance, window_title, window_class, width, height))
        return false;

    if (!this->gfx_->Initialize(window_.GetHWND(), width, height))
        return false;

    InitializePhysics();

    if (!this->InitializeScene())
        return false;

    for (auto* gameObject : gameObjects)
    {
        gameObject->Start();
    }

    return true;
}

bool GameEngine::ProcessMessages()
{
    return this->window_.ProcessMessages();
}

void GameEngine::Update()
{
    static Timer timer;
    deltaTime = timer.GetMilisecondsElapsed();
    timer.Restart();

    for (const auto gameObject : gameObjects)
    {
        gameObject->Update(deltaTime);
    }

    if (this->gfx_->blockInputForImGui || isPaused)
    {
        return;
    }

    physicsEngine->UpdatePhysics(deltaTime);
}

void GameEngine::InitializePhysics()
{
    if (physicsEngine)
    {
        delete physicsEngine;
    }
    physicsEngine = new PhysicsEngine();
    physicsEngine->Initialize(3);
    _bodyInterface = &physicsEngine->GetPhysicsSystem()->GetBodyInterface();
}

void GameEngine::RenderFrame()
{
    float bgcolor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    gfx_->GetDeviceContext()->ClearRenderTargetView(gfx_->GetRenderTargetView().Get(), bgcolor);
    gfx_->GetDeviceContext()->ClearDepthStencilView(gfx_->GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    gfx_->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfx_->GetDeviceContext()->RSSetState(gfx_->GetRasterizerState().Get());
    gfx_->GetDeviceContext()->OMSetDepthStencilState(gfx_->GetDepthStencilState().Get(), 0);
    gfx_->GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
    gfx_->GetDeviceContext()->PSSetSamplers(0, 1, gfx_->GetSamplerState().GetAddressOf());

    gfx_->shaderManager->SetShader(ShaderData("Data\\Shaders\\lightShader.hlsl", PixelType | VertexType));

    for (const auto gameObject : gameObjects)
    {
        if(gameObject->IsVisible())
            gameObject->Render(currentCamera->GetViewMatrix(), currentCamera->GetProjectionMatrix(), _lightDirection);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderGui();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    gfx_->RenderFrame();
}

void GameEngine::RenderGui()
{
}

bool GameEngine::InitializeScene()
{
    return true;
}

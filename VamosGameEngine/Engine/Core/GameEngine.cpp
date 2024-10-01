#include "GameEngine.h"

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

    if (!this->gfx_.Initialize(window_.GetHWND(), width, height))
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

    physicsEngine->UpdatePhysics(deltaTime);
    RenderFrame();
}

void GameEngine::InitializePhysics()
{
    physicsEngine = new PhysicsEngine();
    physicsEngine->Initialize();
}

void GameEngine::RenderFrame()
{
    float bgcolor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    gfx_.GetDeviceContext()->ClearRenderTargetView(gfx_.GetRenderTargetView().Get(), bgcolor);
    gfx_.GetDeviceContext()->ClearDepthStencilView(gfx_.GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    gfx_.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfx_.GetDeviceContext()->RSSetState(gfx_.GetRasterizerState().Get());
    gfx_.GetDeviceContext()->OMSetDepthStencilState(gfx_.GetDepthStencilState().Get(), 0);
    gfx_.GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
    gfx_.GetDeviceContext()->PSSetSamplers(0, 1, gfx_.GetSamplerState().GetAddressOf());

    gfx_.shaderManager->SetShader(ShaderData("Data\\Shaders\\simpleShader.hlsl", PixelType | VertexType));

    for (const auto gameObject : gameObjects)
    {
        if(gameObject->IsVisible())
            gameObject->Render(currentCamera->GetViewMatrix(), currentCamera->GetProjectionMatrix());
    }

    gfx_.RenderFrame();
}

void GameEngine::RenderGui()
{
}

bool GameEngine::InitializeScene()
{
    return true;
}

#include "GameEngine.h"
#include <iostream>

GameEngine::~GameEngine()
{
    for (const auto gameObject : gameObjects)
    {
        gameObject->Destroy();
        delete gameObject;
    }
    delete tempAllocator;
    delete jobSystem;
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

    return true;
}

void GameEngine::InitializePhysics()
{
    // Инициализация аллокатора
    JPH::RegisterDefaultAllocator();

    // Параметры физической системы
    constexpr JPH::uint cMaxBodies = 1024;                  // Максимум тел в мире
    constexpr JPH::uint cNumBodyMutexes = 0;                // Количество мьютексов для тел
    constexpr JPH::uint cMaxBodyPairs = 4096;               // Максимум пар тел для коллизий
    constexpr JPH::uint cMaxContactConstraints = 1024;      // Максимум контактных ограничений

    // Инициализация физической системы
    physicsSystem.Init(cMaxBodies,
                      cNumBodyMutexes,
                      cMaxBodyPairs,
                      cMaxContactConstraints,
                      broadPhaseLayer,
                      objectVsBroadPhaseLayerFilter,
                      objectLayerPairFilter);

    // Инициализация временного аллокатора (создаётся один раз)
    tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10 MB

    // Инициализация JobSystem (создаётся один раз)
    jobSystem = new JPH::JobSystemThreadPool(4, 4, 1000); // 4 рабочих потока
}

bool GameEngine::ProcessMessages()
{
    return this->window_.ProcessMessages();
}

void GameEngine::Update()
{
    deltaTime = timer.GetMilisecondsElapsed();
    timer.Restart();

    for (const auto gameObject : gameObjects)
    {
        gameObject->Update(deltaTime);
    }

    UpdatePhysics(deltaTime);
}

void GameEngine::UpdatePhysics(float deltaTime)
{
    const int cCollisionSteps = 1;         // Количество шагов для коллизий

    // Обновляем физическую систему, передавая аллокатор и систему задач
    JPH::EPhysicsUpdateError result = physicsSystem.Update(deltaTime, cCollisionSteps, tempAllocator, jobSystem);

    if (result != JPH::EPhysicsUpdateError::None)
    {
        // Обрабатываем ошибки обновления физики
        std::cerr << "Physics update error occurred: " << static_cast<int>(result) << std::endl;
    }
}

void GameEngine::RenderFrame()
{
    float bgcolor[] = {0.5f, 0.5f, 0.5f, 1.0f};
    gfx_.GetDeviceContext()->ClearRenderTargetView(gfx_.GetRenderTargetView().Get(), bgcolor);
    gfx_.GetDeviceContext()->ClearDepthStencilView(gfx_.GetDepthStencilView().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                               1.0f, 0);
    gfx_.shaderManager->SetShader(ShaderData("Data\\Shaders\\simpleShader.hlsl", PixelType | VertexType));

    for (const auto gameObject : gameObjects)
    {
        if(gameObject->IsVisible())
            gameObject->Render(currentCamera->GetViewMatrix() * currentCamera->GetProjectionMatrix());
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

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

    if (!this->InitializeScene())
        return false;

    return true;
}

bool GameEngine::ProcessMessages()
{
    return this->window_.ProcessMessages();
}

void GameEngine::RenderFrame()
{
    gfx_.RenderFrame();
}

#pragma once
#include "Engine.h"
#include "Engine/Utilities/Timer.h"

class GameEngine : Engine {
public:
    bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height); 
    bool ProcessMessages();
    void Update();
    void RenderFrame();
private:
    Timer timer;
};
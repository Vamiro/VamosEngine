#include <windows.h>
#include <Engine/Core/GameEngine.h>

#include "Engine/Apps/PongApp/PongApp.h"
#include "Engine/Apps/TestApp/TestApp.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    PongApp game;

    if (game.Start(hInstance, "Vamos Game Engine", "VamosGameEngine", 800, 600))
    {
        while (!game.IsClosed())
        {
            if (!game.ProcessMessages()) ErrorLogger::Log("Failed to process messages.");
            game.Update();
            game.RenderFrame();
        }
    }
}

#include <windows.h>
#include <Engine/Core/GameEngine.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GameEngine game;
    
    if (game.Initialize(hInstance, "Vamos Game Engine", "VamosGameEngine", 800, 600))
    {
        while (game.ProcessMessages())
        {
            game.Update();
            game.RenderFrame();
        }
        if (!game.ProcessMessages())
        {
            ErrorLogger::Log("Failed to process messages.");
        }
    }
}

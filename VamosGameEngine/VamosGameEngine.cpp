#include <windows.h>
#include <Engine/Core/GameEngine.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GameEngine game;
    game.Initialize(hInstance, "Vamos Game Engine", "VamosGameEngine", 800, 600);
}

#pragma once
#include "Window.h"
#include "Input/InputDevice.h"
#include "Engine/Graphics/Graphics.h"

class Engine
{
public:
	Engine();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM LParam);

	[[nodiscard]] Window GetWindow() const { return window_; }
	[[nodiscard]] InputDevice GetInputDevice() const { return input_device_; }
	//[[nodiscard]] Graphics GetGraphics() const { return gfx_; }

protected:
	Window window_;
	InputDevice input_device_;
	Graphics gfx_;
    
};

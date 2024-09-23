#pragma once
#include "Window.h"
#include "Input/InputDevice.h"
#include "Engine/Graphics/Graphics.h"

class Engine
{
public:
	Engine();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM LParam);
	virtual bool ProcessMessages() = 0;
	virtual void Update() = 0;
	virtual void RenderFrame() = 0;
	virtual void RenderGui() = 0;
	virtual virtual bool InitializeScene() = 0;
	[[nodiscard]] Window& GetWindow() { return window_; }
	[[nodiscard]] InputDevice& GetInputDevice() { return input_device_; }
	[[nodiscard]] Graphics& GetGraphics() { return gfx_; }

protected:
	Window window_;
	InputDevice input_device_;
	Graphics gfx_;
};

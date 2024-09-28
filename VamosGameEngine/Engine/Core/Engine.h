#pragma once
#include "Window.h"
#include "Input/InputDevice.h"
#include "Engine/Graphics/Graphics.h"

class Engine
{
public:
	virtual ~Engine() = default;
	Engine();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM LParam);
	virtual bool Start(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) = 0;
	virtual bool ProcessMessages() = 0;
	virtual void Update() = 0;
	virtual void RenderFrame() = 0;
	virtual void RenderGui() = 0;
	virtual bool InitializeScene() = 0;
	[[nodiscard]] Window& GetWindow() { return window_; }
	[[nodiscard]] InputDevice& GetInputDevice() { return input_device_; }
	[[nodiscard]] Graphics& GetGraphics() { return gfx_; }
	bool IsClosed() const { return isClosed; }
	bool IsPaused() const { return isPaused; }

protected:
	bool isClosed = false;
	bool isPaused = false;
	Window window_;
	InputDevice input_device_;
	Graphics gfx_;
};

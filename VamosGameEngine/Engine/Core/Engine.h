#pragma once
#include "Window.h"
#include "Engine/Graphics/Graphics.h"
#include "Engine/Rendering/Camera.h"
#include "Input/InputDevice.h"

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
	bool IsClosed() const { return isClosed; }
	bool IsPaused() const { return isPaused; }

	[[nodiscard]] static Camera* GetCurrentCamera() { return currentCamera; }
	static void SetCurrentCamera(Camera* camera) { currentCamera = camera; }

protected:
	bool isClosed = false;
	bool isPaused = false;
	bool isFocused = true;
	Window window_;

	bool s_in_sizemove = false;
	bool s_minimized = false;

	static InputDevice* input_device_;
	static Graphics* gfx_;
	static Camera* currentCamera;
};

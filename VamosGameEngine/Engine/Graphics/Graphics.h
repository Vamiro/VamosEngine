#pragma once
#include "Adapters.h"
#include "shaders.h"
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/WICTextureLoader.h>

#include "ShaderManager.h"
#include "Engine/Rendering/Camera.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Rendering/GameObject.h"

class Graphics
{
public:
    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
    void SetLookAt(float x, float y, float z);
    void UnSetLookAt();
    bool blockInputForImGui = false;
    std::vector<GameObject*> gameObjects;
    Camera camera;
    ShaderManager* shaderManager;
    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return device; }
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return deviceContext; }
    int GetWindowHeight() const { return windowHeight; }
    int GetWindowWidth() const { return windowWidth; }

private:
    Timer fpsTimer;
    bool lookAt = false;
    bool InitializeDirectX(HWND hwnd);
    bool InitializeShaders();
    int windowWidth = 0;
    int windowHeight = 0;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    VertexShader vertexshader;
    PixelShader pixelshader;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateCullFront;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

    std::unique_ptr<SpriteBatch> spriteBatch;
    std::unique_ptr<SpriteFont> spriteFont;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

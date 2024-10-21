#pragma once
#include <D3D11.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "Adapters.h"
#include "Shaders.h"
#include "ShaderManager.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

class Engine;

class Graphics
{
public:
    Graphics(Engine* engine) : _engine(engine) {};
    ~Graphics();
    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame() const;
    bool blockInputForImGui = false;
    ShaderManager* shaderManager;
    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() { return device; }
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() { return deviceContext; }
    Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return swapChain; }
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() { return renderTargetView; }
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthStencilView() { return depthStencilView; }
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState() { return rasterizerState; }
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState() { return depthStencilState; }
    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() { return samplerState; }
    int GetWindowHeight() const { return windowHeight; }
    int GetWindowWidth() const { return windowWidth; }

    DirectX::SpriteBatch* GetSpriteBatch() const { return spriteBatch.get(); }
    DirectX::SpriteFont* GetSpriteFont() const { return spriteFont.get(); }

private:
    Engine* _engine = nullptr;
    bool InitializeDirectX(HWND hwnd);
    bool InitializeShaders() const;
    bool InitializeGUI(HWND hwnd) const;
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

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

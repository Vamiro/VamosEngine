#pragma once
#include "Adapters.h"
#include "shaders.h"
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/WICTextureLoader.h>
#include "camera.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Rendering/GameObject.h"

class Graphics
{
public:
    GameObject gameObject;
    bool Initialize(HWND hwnd, int width, int height);
    void RenderFrame();
    void SetLookAt(float x, float y, float z);
    void UnSetLookAt();
    Camera camera;
    bool blockInputForImGui = false;

private:
    Timer fpsTimer;
    bool lookAt = false;
    bool InitializeDirectX(HWND hwnd);
    bool InitializeShaders();
    bool InitializeScene();
    int windowWidth = 0;
    int windowHeight = 0;
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
    VertexShader vertexshader;
    PixelShader pixelshader;
    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateCullFront;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;
};

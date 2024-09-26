#include "graphics.h"
#include <D3D11.h>
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "Engine/Core/Engine.h"

//TODO: Implement method for game objects to be rendered in the scene

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
    this->windowHeight = height;
    this->windowWidth = width;
    this->fpsTimer.Start();
    this->shaderManager = new ShaderManager(this);
    
    if (!InitializeDirectX(hwnd))
        return false;

    if (!InitializeShaders())
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
    ImGui::StyleColorsDark();

    return true;
}

void Graphics::RenderFrame()
{
    this->deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    this->deviceContext->RSSetState(this->rasterizerState.Get());
    this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
    this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
    this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

    _engine->RenderGui();

    this->swapChain->Present(0, NULL);
}

void Graphics::SetLookAt(float x, float y, float z)
{
    this->lookAt = true;
}

void Graphics::UnSetLookAt()
{
    this->lookAt = false;
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
    try
    {
        std::vector<AdapterData> adapters = Adapters::GetAdapters();

        if (adapters.size() < 1)
        {
            ErrorLogger::Log("No IDXGI Adapters found.");
            return false;
        }

        DXGI_SWAP_CHAIN_DESC scd = {0};

        scd.BufferDesc.Width = this->windowWidth;
        scd.BufferDesc.Height = this->windowHeight;
        scd.BufferDesc.RefreshRate.Numerator = 60;
        scd.BufferDesc.RefreshRate.Denominator = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

        scd.SampleDesc.Count = 1;
        scd.SampleDesc.Quality = 0;

        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 1;
        scd.OutputWindow = hwnd;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        HRESULT hr;
        hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
                                           D3D_DRIVER_TYPE_UNKNOWN,
                                           NULL, //FOR SOFTWARE DRIVER TYPE
                                           D3D11_CREATE_DEVICE_DEBUG, //FLAGS FOR RUNTIME LAYERS
                                           NULL, //FEATURE LEVELS ARRAY
                                           0, //# OF FEATURE LEVELS IN ARRAY
                                           D3D11_SDK_VERSION,
                                           &scd, //Swapchain description
                                           this->swapChain.GetAddressOf(), //Swapchain Address
                                           this->device.GetAddressOf(), //Device Address
                                           NULL, //Supported feature level
                                           this->deviceContext.GetAddressOf()); //Device Context Address

        ErrorLogger::Log(hr, "Failed to create device and swapchain.");
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                        reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        ErrorLogger::Log(hr, "GetBuffer Failed.");

        hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create render target view.");

        //Describe our Depth/Stencil Buffer
        CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth,
                                                      this->windowHeight);
        depthStencilTextureDesc.MipLevels = 1;
        depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");

        hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL,
                                                  this->depthStencilView.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil view.");

        this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

        //Create depth stencil state
        CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
        depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

        hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil state.");

        //Create & set the Viewport
        CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth),
                                 static_cast<float>(this->windowHeight));;
        this->deviceContext->RSSetViewports(1, &viewport);

        //Create Rasterizer State
        CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
        hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create rasterizer state.");

        //Create Rasterizer State for culling front
        CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
        rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
        hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront,
                                                 this->rasterizerStateCullFront.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create rasterizer state.");

        //Create Blend State
        D3D11_RENDER_TARGET_BLEND_DESC rtbd = {0};
        rtbd.BlendEnable = true;
        rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
        rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
        rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
        rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
        rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
        rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blendDesc = {0};
        blendDesc.RenderTarget[0] = rtbd;

        hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create blend state.");

        spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
        spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\pixel.spritefont");

        //Create sampler description for sampler state
        CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
        ErrorLogger::Log(hr, "Failed to create sampler state.");
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}

bool Graphics::InitializeShaders()
{
    shaderManager->InitShader(ShaderData("Data\\Shaders\\simpleShader.hlsl", PixelType | VertexType));
    return true;
}

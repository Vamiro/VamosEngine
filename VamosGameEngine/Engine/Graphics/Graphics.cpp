#include "Graphics.h"


Graphics::~Graphics()
{
    delete shaderManager;
    shaderManager = nullptr;
}

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
    this->windowHeight = height;
    this->windowWidth = width;
    this->shaderManager = new ShaderManager(this);
    
    if (!InitializeDirectX(hwnd))
        return false;

    InitializeShaders();

    InitializeGUI(hwnd);

    return true;
}

void Graphics::RenderFrame() const
{
    this->swapChain->Present(0, NULL);
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

        DXGI_SWAP_CHAIN_DESC scd = {};

        scd.BufferCount                           = 2;                                      //Number of back buffers
        scd.BufferDesc.Width                      = this->windowWidth;                      //Buffer width
        scd.BufferDesc.Height                     = this->windowHeight;                     //Buffer height
        scd.BufferDesc.Format                     = DXGI_FORMAT_R8G8B8A8_UNORM;             //Buffer display format
        scd.BufferDesc.RefreshRate.Numerator      = 144;                                    //Refresh rate (144Hz)
        scd.BufferDesc.RefreshRate.Denominator    = 1;                                      //Refresh rate
        scd.BufferDesc.ScanlineOrdering           = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;   //Scanline order
        scd.BufferDesc.Scaling                    = DXGI_MODE_SCALING_CENTERED;             //Scaling
        scd.BufferUsage                           = DXGI_USAGE_RENDER_TARGET_OUTPUT;        //Buffer usage
        scd.OutputWindow                          = hwnd;                                   //Output window
        scd.Windowed                              = TRUE;                                   //Windowed/full-screen mode
        scd.SwapEffect                            = DXGI_SWAP_EFFECT_DISCARD;               //Swap effect
        scd.Flags                                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //Flags
        scd.SampleDesc.Count                      = 1;                                      //Number of samples
        scd.SampleDesc.Quality                    = 0;                                      //Sample quality

        HRESULT hr;
        D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };
        hr = D3D11CreateDeviceAndSwapChain(
            adapters[0].pAdapter,                //IDXGI Adapter
            D3D_DRIVER_TYPE_UNKNOWN,             //Driver Type
            NULL,                                //FOR SOFTWARE DRIVER TYPE
            D3D11_CREATE_DEVICE_DEBUG,           //FLAGS FOR RUNTIME LAYERS
            featureLevel,                        //FEATURE LEVELS ARRAY
            1,                                   //# OF FEATURE LEVELS IN ARRAY
            D3D11_SDK_VERSION,                   //SDK Version
            &scd,                                //Swapchain description
            this->swapChain.GetAddressOf(),      //Swapchain Address
            this->device.GetAddressOf(),         //Device Address
            NULL,                                //Supported feature level
            this->deviceContext.GetAddressOf()); //Device Context Address
        ErrorLogger::Log(hr, "Failed to create device and swapchain.");

        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
        hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
        ErrorLogger::Log(hr, "GetBuffer Failed.");

        hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create render target view.");

        CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
        depthStencilTextureDesc.MipLevels = 1;
        depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil buffer.");

        hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil view.");

        this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

        CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
        depthstencildesc.DepthEnable = TRUE;
        depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;

        hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create depth stencil state.");

        CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth),
                                 static_cast<float>(this->windowHeight));;
        this->deviceContext->RSSetViewports(1, &viewport);

        CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = TRUE;
        rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = TRUE;

        hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create rasterizer state.");

        CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
        rasterizerDesc_CullFront.CullMode = D3D11_CULL_FRONT;
        hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerStateCullFront.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create rasterizer state.");

        D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {0};
        renderTargetBlendDesc.BlendEnable                = TRUE;                                 //Enable blending
        renderTargetBlendDesc.SrcBlend                   = D3D11_BLEND_SRC_ALPHA;                //How to blend the new color
        renderTargetBlendDesc.DestBlend                  = D3D11_BLEND_INV_SRC_ALPHA;            //How to blend the existing color
        renderTargetBlendDesc.BlendOp                    = D3D11_BLEND_OP_ADD;                   //How to combine the two colors
        renderTargetBlendDesc.SrcBlendAlpha              = D3D11_BLEND_ONE;                      //How to blend the new alpha
        renderTargetBlendDesc.DestBlendAlpha             = D3D11_BLEND_ZERO;                     //How to blend the existing alpha
        renderTargetBlendDesc.BlendOpAlpha               = D3D11_BLEND_OP_ADD;                   //How to combine the two alphas
        renderTargetBlendDesc.RenderTargetWriteMask      = D3D11_COLOR_WRITE_ENABLE_ALL;         //Which color channels to write to

        D3D11_BLEND_DESC blendDesc = {};
        blendDesc.RenderTarget[0] = renderTargetBlendDesc;
        hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create blend state.");

        spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
        spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\pixel.spritefont");

        CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
        ErrorLogger::Log(hr, "Failed to create sampler state.");
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}

bool Graphics::InitializeShaders() const
{
    shaderManager->InitShader(ShaderData("Data\\Shaders\\lightShader.hlsl", PixelType | VertexType));
    return true;

    // std::vector<std::string> shaderNames = {"lightShader", "simpleShader"};
    // for (const auto& name : shaderNames)
    // {
    //     shaderManager->InitShader(ShaderData("Data\\CompiledShaders\\" + name + "_ps.cso", PixelType));
    //     shaderManager->InitShader(ShaderData("Data\\CompiledShaders\\" + name + "_vs.cso", VertexType));
    // }
    // return true;
}

bool Graphics::InitializeGUI(const HWND hwnd) const
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
    ImGui::StyleColorsDark();
    return true;
}

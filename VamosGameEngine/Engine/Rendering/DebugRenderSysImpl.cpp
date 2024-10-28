#include "DebugRenderSysImpl.h"
#include "Camera.h"
#include "VertexPositionNormalBinormalTangentColorTex.h"
#include "VertexPositionTex.h"
#include "Engine/Core/Engine.h"


using namespace DirectX::SimpleMath;


void DebugRenderSysImpl::InitPrimitives()
{
    HRESULT res;
    ID3DBlob* errorCode = nullptr;

    res = D3DCompileFromFile(L"Data\\Shaders\\Simple.hlsl",
                             nullptr /*macros*/,
                             nullptr /*include*/,
                             "VSMain",
                             "vs_5_0",
                             D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
                             0,
                             &vertexPrimCompResult,
                             &errorCode);

    if (errorCode) errorCode->Release();

    res = D3DCompileFromFile(L"Data\\Shaders\\Simple.hlsl",
                             nullptr /*macros*/,
                             nullptr /*include*/,
                             "PSMain",
                             "ps_5_0",
                             D3DCOMPILE_PACK_MATRIX_ROW_MAJOR,
                             0,
                             &pixelPrimCompResult,
                             &errorCode);

    if (errorCode) errorCode->Release();

    Engine::GetGraphics().GetDevice()->CreateVertexShader(vertexPrimCompResult->GetBufferPointer(),
                                                          vertexPrimCompResult->GetBufferSize(), nullptr,
                                                          &vertexPrimShader);
    Engine::GetGraphics().GetDevice()->CreatePixelShader(pixelPrimCompResult->GetBufferPointer(),
                                                         pixelPrimCompResult->GetBufferSize(), nullptr,
                                                         &pixelPrimShader);

    layoutPrim = VertexPositionColor::GetLayout(vertexPrimCompResult);


    D3D11_BUFFER_DESC bufDesc = {};
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufDesc.MiscFlags = 0;
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufDesc.ByteWidth = MaxPointsCount * VertexPositionColor::Stride;

    Engine::GetGraphics().GetDevice()->CreateBuffer(&bufDesc, nullptr, &verticesPrim);
    bufPrimStride = VertexPositionColor::Stride;


    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.FillMode = D3D11_FILL_WIREFRAME;

    res = Engine::GetGraphics().GetDevice()->CreateRasterizerState(&rastDesc, &rastState);


    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = false;

    Engine::GetGraphics().GetDevice()->CreateDepthStencilState(&depthDesc, &depthState);

    lines.reserve(100);
}


void DebugRenderSysImpl::InitQuads()
{
    quadProjMatrix = Matrix::CreateOrthographicOffCenter(0, static_cast<float>(Engine::GetGraphics().GetWindowWidth()),
                                                         static_cast<float>(Engine::GetGraphics().GetWindowHeight()), 0,
                                                         0.1f, 1000.0f);

    ID3DBlob* errorCode = nullptr;

    D3DCompileFromFile(L"Data\\Shaders\\TexturedShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0",
                       D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &vertexQuadCompResult, &errorCode);
    HRESULT hr = Engine::GetGraphics().GetDevice()->CreateVertexShader(vertexQuadCompResult->GetBufferPointer(),
                                                          vertexQuadCompResult->GetBufferSize(), nullptr,
                                                          &vertexQuadShader);
    ErrorLogger::Log(hr, "Failed to create vertex shader");
    if (errorCode)errorCode->Release();

    D3DCompileFromFile(L"Data\\Shaders\\TexturedShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0",
                       D3DCOMPILE_PACK_MATRIX_ROW_MAJOR, 0, &pixelQuadCompResult, &errorCode);
    Engine::GetGraphics().GetDevice()->CreatePixelShader(pixelQuadCompResult->GetBufferPointer(),
                                                         pixelQuadCompResult->GetBufferSize(), nullptr,
                                                         &pixelQuadShader);

    if (errorCode)errorCode->Release();


    quadLayout = VertexPositionTex::GetLayout(vertexQuadCompResult);
    quadBindingStride = VertexPositionTex::Stride;

    quads.reserve(10);

    auto points = new Vector4[8]{
        Vector4(1, 1, 0.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f),
        Vector4(0, 1, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f),
        Vector4(1, 0, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f),
        Vector4(0, 0, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    };

    D3D11_BUFFER_DESC bufDesc = {};
    {
        bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufDesc.CPUAccessFlags = 0;
        bufDesc.MiscFlags = 0;
        bufDesc.Usage = D3D11_USAGE_DEFAULT;
        bufDesc.ByteWidth = sizeof(float) * 4 * 8;
    }

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = points;

    Engine::GetGraphics().GetDevice()->CreateBuffer(&bufDesc, &subData, &quadBuf);

    delete[] points;

    float borderCol[] = {1.0f, 0.0f, 0.0f, 1.0f};

    D3D11_SAMPLER_DESC samplDesc = {};
    {
        samplDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplDesc.BorderColor[0] = 1.0f;
        samplDesc.BorderColor[1] = 0.0f;
        samplDesc.BorderColor[2] = 0.0f;
        samplDesc.BorderColor[3] = 1.0f;
        samplDesc.MaxLOD = static_cast<float>(INT_MAX);
    }
    Engine::GetGraphics().GetDevice()->CreateSamplerState(&samplDesc, &quadSampler);

    D3D11_RASTERIZER_DESC rastDesc = {};
    {
        rastDesc.CullMode = D3D11_CULL_FRONT;
        rastDesc.FillMode = D3D11_FILL_SOLID;
    }

    Engine::GetGraphics().GetDevice()->CreateRasterizerState(&rastDesc, &quadRastState);
}


void DebugRenderSysImpl::DrawPrimitives()
{
    if (isPrimitivesDirty)
    {
        UpdateLinesBuffer();
        pointsCount = lines.size();

        isPrimitivesDirty = false;
    }

    auto mat = (Engine::GetCurrentCamera()->GetViewMatrix() * Engine::GetCurrentCamera()->GetProjectionMatrix());

    Engine::GetGraphics().GetDeviceContext()->UpdateSubresource(constBuf, 0, nullptr, &mat, 0, 0);

    Engine::GetGraphics().GetDeviceContext()->OMSetDepthStencilState(depthState, 0);
    Engine::GetGraphics().GetDeviceContext()->RSSetState(rastState);

    Engine::GetGraphics().GetDeviceContext()->VSSetShader(vertexPrimShader, nullptr, 0);
    Engine::GetGraphics().GetDeviceContext()->PSSetShader(pixelPrimShader, nullptr, 0);

    Engine::GetGraphics().GetDeviceContext()->IASetInputLayout(layoutPrim);
    Engine::GetGraphics().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    const UINT offset = 0;
    Engine::GetGraphics().GetDeviceContext()->IASetVertexBuffers(0, 1, &verticesPrim, &bufPrimStride, &offset);

    Engine::GetGraphics().GetDeviceContext()->VSSetConstantBuffers(0, 1, &constBuf);

    Engine::GetGraphics().GetDeviceContext()->Draw(pointsCount, 0);
}


void DebugRenderSysImpl::DrawQuads()
{
    if (quads.empty()) return;

    Engine::GetGraphics().GetDeviceContext()->OMSetDepthStencilState(depthState, 0);
    Engine::GetGraphics().GetDeviceContext()->RSSetState(quadRastState);

    Engine::GetGraphics().GetDeviceContext()->VSSetShader(vertexQuadShader, nullptr, 0);
    Engine::GetGraphics().GetDeviceContext()->PSSetShader(pixelQuadShader, nullptr, 0);

    Engine::GetGraphics().GetDeviceContext()->IASetInputLayout(quadLayout);
    Engine::GetGraphics().GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    const UINT offset = 0;
    Engine::GetGraphics().GetDeviceContext()->IASetVertexBuffers(0, 1, &quadBuf, &quadBindingStride, &offset);

    Engine::GetGraphics().GetDeviceContext()->VSSetConstantBuffers(0, 1, &constBuf);

    for (auto& quad : quads)
    {
        auto mat = quad.TransformMat * quadProjMatrix;
        Engine::GetGraphics().GetDeviceContext()->UpdateSubresource(constBuf, 0, nullptr, &mat, 0, 0);

        Engine::GetGraphics().GetDeviceContext()->PSSetShaderResources(0, 1, &quad.Srv);
        Engine::GetGraphics().GetDeviceContext()->PSSetSamplers(0, 1, &quadSampler);

        Engine::GetGraphics().GetDeviceContext()->Draw(4, 0);
    }
}


void DebugRenderSysImpl::UpdateLinesBuffer()
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    Engine::GetGraphics().GetDeviceContext()->Map(verticesPrim, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    memcpy(mapped.pData, lines.data(), lines.size() * VertexPositionColor::Stride);

    Engine::GetGraphics().GetDeviceContext()->Unmap(verticesPrim, 0);
}


DebugRenderSysImpl::~DebugRenderSysImpl()
{
}


DebugRenderSysImpl::DebugRenderSysImpl()
{
}

void DebugRenderSysImpl::Initialize()
{
    D3D11_BUFFER_DESC constDesc = {};
    constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constDesc.CPUAccessFlags = 0;
    constDesc.MiscFlags = 0;
    constDesc.Usage = D3D11_USAGE_DEFAULT;
    constDesc.ByteWidth = sizeof(Matrix);

    Engine::GetGraphics().GetDevice()->CreateBuffer(&constDesc, nullptr, &constBuf);

    InitPrimitives();
    InitQuads();
}

void DebugRenderSysImpl::Draw()
{
    Engine::GetGraphics().GetDeviceContext()->ClearState();
    Engine::GetGraphics().RestoreTargets();

    D3D11_VIEWPORT viewport = {
        0,
        0,
        static_cast<float>(Engine::GetGraphics().GetWindowWidth()),
        static_cast<float>(Engine::GetGraphics().GetWindowHeight()),
        0,
        1
    };

    Engine::GetGraphics().GetDeviceContext()->RSSetViewports(1, &viewport);

    DrawPrimitives();
    DrawQuads();
}


void DebugRenderSysImpl::Clear()
{
    lines.clear();
    quads.clear();
}


void DebugRenderSysImpl::DrawBoundingBox(const DirectX::BoundingBox& box)
{
    Vector3 corners[8];

    box.GetCorners(&corners[0]);

    DrawLine(corners[0], corners[1], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[1], corners[2], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[2], corners[3], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[3], corners[0], Color(0.0f, 1.0f, 0.0f, 1.0f));

    DrawLine(corners[4], corners[5], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[5], corners[6], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[6], corners[7], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[7], corners[4], Color(0.0f, 1.0f, 1.0f, 1.0f));

    DrawLine(corners[0], corners[4], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[1], corners[5], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[2], corners[6], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[3], corners[7], Color(0.0f, 0.0f, 1.0f, 1.0f));
}


void DebugRenderSysImpl::DrawBoundingBox(const DirectX::BoundingBox& box, const DirectX::SimpleMath::Matrix& transform)
{
    Vector3 corners[8];
    box.GetCorners(&corners[0]);

    for (auto& corner : corners)
    {
        corner = Vector3::Transform(corner, transform);
    }

    DrawLine(corners[0], corners[1], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[1], corners[2], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[2], corners[3], Color(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(corners[3], corners[0], Color(0.0f, 1.0f, 0.0f, 1.0f));

    DrawLine(corners[4], corners[5], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[5], corners[6], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[6], corners[7], Color(0.0f, 1.0f, 1.0f, 1.0f));
    DrawLine(corners[7], corners[4], Color(0.0f, 1.0f, 1.0f, 1.0f));

    DrawLine(corners[0], corners[4], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[1], corners[5], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[2], corners[6], Color(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(corners[3], corners[7], Color(0.0f, 0.0f, 1.0f, 1.0f));
}

void DebugRenderSysImpl::DrawBoundingBox(const DirectX::BoundingBox& box, const DirectX::SimpleMath::Matrix& transform, const DirectX::SimpleMath::Color& color)
{
    Vector3 corners[8];
    box.GetCorners(&corners[0]);

    for (auto& corner : corners)
    {
        corner = Vector3::Transform(corner, transform);
    }

    DrawLine(corners[0], corners[1], color);
    DrawLine(corners[1], corners[2], color);
    DrawLine(corners[2], corners[3], color);
    DrawLine(corners[3], corners[0], color);

    DrawLine(corners[4], corners[5], color);
    DrawLine(corners[5], corners[6], color);
    DrawLine(corners[6], corners[7], color);
    DrawLine(corners[7], corners[4], color);

    DrawLine(corners[0], corners[4], color);
    DrawLine(corners[1], corners[5], color);
    DrawLine(corners[2], corners[6], color);
    DrawLine(corners[3], corners[7], color);
}

void DebugRenderSysImpl::DrawLine(const DirectX::SimpleMath::Vector3& pos0, const DirectX::SimpleMath::Vector3& pos1,
                                  const DirectX::SimpleMath::Color& color)
{
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos0.x, pos0.y, pos0.z, 1.0f),
            color.ToVector4()
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos1.x, pos1.y, pos1.z, 1.0f),
            color.ToVector4()
        });

    isPrimitivesDirty = true;
}


void DebugRenderSysImpl::DrawArrow(const DirectX::SimpleMath::Vector3& p0, const DirectX::SimpleMath::Vector3& p1,
                                   const DirectX::SimpleMath::Color& color, const DirectX::SimpleMath::Vector3& n)
{
    DrawLine(p0, p1, color);

    Vector3 a = Vector3::Lerp(p0, p1, 0.85f);

    Vector3 diff = p1 - p0;
    Vector3 side = n.Cross(diff) * 0.05f;

    DrawLine(a + side, p1, color);
    DrawLine(a - side, p1, color);
}


void DebugRenderSysImpl::DrawPoint(const DirectX::SimpleMath::Vector3& pos, const float& size)
{
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x + size, pos.y, pos.z, 1.0f),
            Vector4(1.0f, 0.0f, 0.0f, 1.0f)
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x - size, pos.y, pos.z, 1.0f),
            Vector4(1.0f, 0.0f, 0.0f, 1.0f)
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x, pos.y + size, pos.z, 1.0f),
            Vector4(0.0f, 1.0f, 0.0f, 1.0f)
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x, pos.y - size, pos.z, 1.0f),
            Vector4(0.0f, 1.0f, 0.0f, 1.0f)
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x, pos.y, pos.z + size, 1.0f),
            Vector4(0.0f, 0.0f, 1.0f, 1.0f)
        });
    lines.emplace_back(VertexPositionColor
        {
            Vector4(pos.x, pos.y, pos.z - size, 1.0f),
            Vector4(0.0f, 0.0f, 1.0f, 1.0f)
        });

    isPrimitivesDirty = true;
}


void DebugRenderSysImpl::DrawCircle(const double& radius, const DirectX::SimpleMath::Color& color,
                                    const DirectX::SimpleMath::Matrix& transform, int density)
{
    double angleStep = DirectX::XM_PI * 2 / density;

    for (int i = 0; i < density; i++)
    {
        auto point0X = radius * cos(angleStep * i);
        auto point0Y = radius * sin(angleStep * i);

        auto point1X = radius * cos(angleStep * (i + 1));
        auto point1Y = radius * sin(angleStep * (i + 1));

        auto p0 = Vector3::Transform(Vector3(static_cast<float>(point0X), static_cast<float>(point0Y), 0), transform);
        auto p1 = Vector3::Transform(Vector3(static_cast<float>(point1X), static_cast<float>(point1Y), 0), transform);

        DrawLine(p0, p1, color);
    }
}


void DebugRenderSysImpl::DrawSphere(const double& radius, const DirectX::SimpleMath::Color& color,
                                    const DirectX::SimpleMath::Matrix& transform, int density)
{
    DrawCircle(radius, color, transform, density);
    DrawCircle(radius, color, Matrix::CreateRotationX(DirectX::XM_PIDIV2) * transform, density);
    DrawCircle(radius, color, Matrix::CreateRotationY(DirectX::XM_PIDIV2) * transform, density);
}

void DebugRenderSysImpl::DrawPlane(const DirectX::SimpleMath::Vector4& p, const DirectX::SimpleMath::Color& color,
                                   float sizeWidth, float sizeNormal, bool drawCenterCross)
{
    Vector3 dir = Vector3(p.x, p.y, p.z);
    if (dir.Length() == 0.0f) return;
    dir.Normalize();

    Vector3 up = Vector3(0, 0, 1);
    Vector3 right = dir.Cross(up);
    if (right.Length() < 0.01f)
    {
        up = Vector3(0, 1, 0);
        right = dir.Cross(up);
    }
    right.Normalize();

    up = dir.Cross(up);

    Vector3 pos = -dir * p.w;

    Vector3 leftPoint = pos - right * sizeWidth;
    Vector3 rightPoint = pos + right * sizeWidth;
    Vector3 downPoint = pos - up * sizeWidth;
    Vector3 upPoint = pos + up * sizeWidth;

    DrawLine(leftPoint + up * sizeWidth, rightPoint + up * sizeWidth, color);
    DrawLine(leftPoint - up * sizeWidth, rightPoint - up * sizeWidth, color);
    DrawLine(downPoint - right * sizeWidth, upPoint - right * sizeWidth, color);
    DrawLine(downPoint + right * sizeWidth, upPoint + right * sizeWidth, color);


    if (drawCenterCross)
    {
        DrawLine(leftPoint, rightPoint, color);
        DrawLine(downPoint, upPoint, color);
    }

    DrawPoint(pos, 0.5f);
    DrawArrow(pos, pos + dir * sizeNormal, color, right);
}

std::vector<Vector4> GetFrustumCornersWorldSpace(const Matrix& view, const Matrix& proj)
{
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();

    std::vector<Vector4> frustumCorners;
    frustumCorners.reserve(8);
    for (unsigned int x = 0; x < 2; ++x)
    {
        for (unsigned int y = 0; y < 2; ++y)
        {
            for (unsigned int z = 0; z < 2; ++z)
            {
                const Vector4 pt = Vector4::Transform(Vector4(2.0f * static_cast<float>(x) - 1.0f,
                                                              2.0f * static_cast<float>(y) - 1.0f,
                                                              static_cast<float>(z), 1.0f), inv);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

void DebugRenderSysImpl::DrawFrustrum(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    const auto corners = GetFrustumCornersWorldSpace(view, proj);

    //for (int i = 0; i < corners.size(); ++i) {
    //	DrawPoint(corners[i].ToVec3(), 2.0f * (i+1));
    //}

    auto invView = view.Invert();
    DrawPoint(invView.Translation(), 1.0f);

    DrawLine(Vector3(corners[0].x, corners[0].y, corners[0].z), Vector3(corners[1].x, corners[1].y, corners[1].z),
             Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(Vector3(corners[2].x, corners[2].y, corners[2].z), Vector3(corners[3].x, corners[3].y, corners[3].z),
             Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(Vector3(corners[4].x, corners[4].y, corners[4].z), Vector3(corners[5].x, corners[5].y, corners[5].z),
             Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    DrawLine(Vector3(corners[6].x, corners[6].y, corners[6].z), Vector3(corners[7].x, corners[7].y, corners[7].z),
             Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    DrawLine(Vector3(corners[0].x, corners[0].y, corners[0].z), Vector3(corners[2].x, corners[2].y, corners[2].z),
             Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[1].x, corners[1].y, corners[1].z), Vector3(corners[3].x, corners[3].y, corners[3].z),
             Vector4(0.0f, 0.5f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[4].x, corners[4].y, corners[4].z), Vector3(corners[6].x, corners[6].y, corners[6].z),
             Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[5].x, corners[5].y, corners[5].z), Vector3(corners[7].x, corners[7].y, corners[7].z),
             Vector4(0.0f, 0.5f, 0.0f, 1.0f));

    DrawLine(Vector3(corners[0].x, corners[0].y, corners[0].z), Vector3(corners[4].x, corners[4].y, corners[4].z),
             Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[1].x, corners[1].y, corners[1].z), Vector3(corners[5].x, corners[5].y, corners[5].z),
             Vector4(0.5f, 0.0f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[2].x, corners[2].y, corners[2].z), Vector3(corners[6].x, corners[6].y, corners[6].z),
             Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    DrawLine(Vector3(corners[3].x, corners[3].y, corners[3].z), Vector3(corners[7].x, corners[7].y, corners[7].z),
             Vector4(0.5f, 0.0f, 0.0f, 1.0f));
}


void DebugRenderSysImpl::DrawTextureOnScreen(ID3D11ShaderResourceView* tex, int x, int y, int width, int height,
                                             int zOrder)
{
    if (quads.size() >= QuadMaxDrawCount) return;

    QuadInfo quad = {};
    quad.Srv = tex;
    quad.TransformMat = Matrix::CreateScale(static_cast<float>(width), static_cast<float>(height), 1.0f)
        * Matrix::CreateTranslation(static_cast<float>(x), static_cast<float>(y), static_cast<float>(zOrder));

    quads.emplace_back(quad);
}

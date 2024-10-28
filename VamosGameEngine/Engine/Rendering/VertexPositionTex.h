#pragma once
#include <SimpleMath.h>
#include <d3d11.h>

struct VertexPositionTex
{
    VertexPositionTex() = default;
    VertexPositionTex(DirectX::SimpleMath::Vector4 position, DirectX::SimpleMath::Vector4 texture) : Position(position), Texture(texture)
    {}

    DirectX::SimpleMath::Vector4 Position;
    DirectX::SimpleMath::Vector4 Texture;

    static const UINT Stride = 32;

    static ID3D11InputLayout* GetLayout(ID3DBlob* signature);
};

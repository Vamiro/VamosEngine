#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

struct VertexPositionNormalBinormalTangentColorTex
{
    VertexPositionNormalBinormalTangentColorTex() = default;
    VertexPositionNormalBinormalTangentColorTex(DirectX::SimpleMath::Vector4 position,
                                                DirectX::SimpleMath::Vector4 normales,
                                                DirectX::SimpleMath::Vector4 binormales,
                                                DirectX::SimpleMath::Vector4 tagnents,
                                                DirectX::SimpleMath::Vector4 color, DirectX::SimpleMath::Vector4 tex):
        pos(position),normal(normales),binormal(binormales),color(color),tex(tex) {

    }

    DirectX::SimpleMath::Vector4 pos;
    DirectX::SimpleMath::Vector4 normal;
    DirectX::SimpleMath::Vector4 binormal;
    DirectX::SimpleMath::Vector4 tangent;
    DirectX::SimpleMath::Vector4 color;
    DirectX::SimpleMath::Vector4 tex;

    static const UINT Stride = 96;

    static ID3D11InputLayout* GetLayout(ID3DBlob* signature);


};

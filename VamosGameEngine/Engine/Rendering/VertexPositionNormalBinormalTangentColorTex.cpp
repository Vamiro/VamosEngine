#include "VertexPositionNormalBinormalTangentColorTex.h"
#include "Engine/Core/Engine.h"

ID3D11InputLayout* VertexPositionNormalBinormalTangentColorTex::GetLayout(ID3DBlob* signature)
{
    // Layout from VertexShader input signature
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC {
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
            D3D11_INPUT_ELEMENT_DESC {
                "BINORMAL",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0},
                D3D11_INPUT_ELEMENT_DESC {
                    "TANGENT",
                    0,
                    DXGI_FORMAT_R32G32B32A32_FLOAT,
                    0,
                    D3D11_APPEND_ALIGNED_ELEMENT,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0},
                    D3D11_INPUT_ELEMENT_DESC {
                        "COLOR",
                        0,
                        DXGI_FORMAT_R32G32B32A32_FLOAT,
                        0,
                        D3D11_APPEND_ALIGNED_ELEMENT,
                        D3D11_INPUT_PER_VERTEX_DATA,
                        0},
                        D3D11_INPUT_ELEMENT_DESC {
                            "TEXCOORD",
                            0,
                            DXGI_FORMAT_R32G32B32A32_FLOAT,
                            0,
                            D3D11_APPEND_ALIGNED_ELEMENT,
                            D3D11_INPUT_PER_VERTEX_DATA,
                            0}
    };
    ID3D11InputLayout* layout;
    HRESULT hr = Engine::GetGraphics().GetDevice()->CreateInputLayout(inputElements, 6, signature->GetBufferPointer(),
                                                         signature->GetBufferSize(), &layout);

    ErrorLogger::Log(hr, "Failed to create input layout for VertexPositionNormalBinormalTangentColorTex.");
    return layout;

}



#pragma once
#include "Engine/Utilities/ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
    bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath,
                    D3D11_INPUT_ELEMENT_DESC* desc, UINT numElem);

    [[nodiscard]] ID3D11VertexShader* GetShader() const { return shader.Get(); }
    [[nodiscard]] ID3D10Blob* GetBuffer() const { return shader_buffer.Get(); }
    [[nodiscard]] ID3D11InputLayout* GetInputLayout() const { return inputLayout.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
    Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
    bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);

    [[nodiscard]] ID3D11PixelShader* GetShader() const { return shader.Get(); }
    [[nodiscard]] ID3D10Blob* GetBuffer() const { return shader_buffer.Get(); }
    
private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
    Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
};

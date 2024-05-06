#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath,
                              D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElem)
{
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring errorMsg = L"Failed to load shader: ";
        errorMsg += shaderpath;
        ErrorLogger::Log(hr, errorMsg);
        return false;
    }

    hr = device->CreateVertexShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL,
                                    this->shader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring errorMsg = L"Failed to create vertex shader: ";
        errorMsg += shaderpath;
        ErrorLogger::Log(hr, errorMsg);
        return false;
    }


    hr = device->CreateInputLayout(layoutDesc, numElem, this->shader_buffer->GetBufferPointer(),
                                   this->shader_buffer->GetBufferSize(), this->inputLayout.GetAddressOf());
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create input layout.");
        return false;
    }


    return true;
}

bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath)
{
    HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shader_buffer.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring errorMsg = L"Failed to load shader: ";
        errorMsg += shaderpath;
        ErrorLogger::Log(hr, errorMsg);
        return false;
    }

    hr = device->CreatePixelShader(this->shader_buffer->GetBufferPointer(), this->shader_buffer->GetBufferSize(), NULL,
                                   this->shader.GetAddressOf());
    if (FAILED(hr))
    {
        std::wstring errorMsg = L"Failed to create pixel shader: ";
        errorMsg += shaderpath;
        ErrorLogger::Log(hr, errorMsg);
        return false;
    }
    return true;
}
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Engine/Utilities/ErrorLogger.h"

struct CB_VS_VertexShader
{
    DirectX::XMFLOAT4X4 world;
    DirectX::XMFLOAT4X4 cameraView;
    DirectX::XMFLOAT4X4 cameraProj;
    DirectX::XMFLOAT4X4 InvWorldView;
};

struct CB_PS_PixelShader
{
    DirectX::XMFLOAT4 objectColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 1.0f };
};

template <class T>
class ConstantBuffer
{
    ConstantBuffer(const ConstantBuffer<T>& rhs);

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
    ID3D11DeviceContext* deviceContext = nullptr;

public:
    ConstantBuffer()
    {
    }

    T data;

    ID3D11Buffer* Get() const
    {
        return buffer.Get();
    }

    ID3D11Buffer* const* GetAddressOf() const
    {
        return buffer.GetAddressOf();
    }

    HRESULT Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    {
        if (this->buffer.Get() != nullptr)
            buffer.Reset();
        this->deviceContext = deviceContext;

        D3D11_BUFFER_DESC desc;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16)));
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
        return hr;
    }

    bool ApplyChanges(UINT index)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = this->deviceContext->Map(buffer.Get(), index, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if (FAILED(hr))
        {
            ErrorLogger::Log(hr, "Failed to map constant buffer.");
            return false;
        }
        CopyMemory(mappedResource.pData, &data, sizeof(T));
        this->deviceContext->Unmap(buffer.Get(), index);
        return true;
    }
};

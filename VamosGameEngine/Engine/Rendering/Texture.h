#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <assimp/material.h>
#include "Engine/Utilities/ErrorLogger.h"
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/DDSTextureLoader.h>

enum class TextureStorageType
{
    Invalid,
    None,
    EmbeddedIndexCompressed,
    EmbeddedIndexNonCompressed,
    EmbeddedCompressed,
    EmbeddedNonCompressed,
    Disk
};

class Texture
{
public:
    Texture(ID3D11Device* device, const DirectX::SimpleMath::Color& color, aiTextureType type);
    Texture(ID3D11Device* device, const std::string & filePath,aiTextureType type);
    Texture(ID3D11Device* device, const DirectX::SimpleMath::Color* colorData, UINT width, UINT height, aiTextureType type); //Generate texture of specific color data
    Texture(ID3D11Device* device,const uint8_t* pData,size_t size,aiTextureType type);
    aiTextureType GetType();
    ID3D11ShaderResourceView* GetTextureResourceView();
    ID3D11ShaderResourceView** GetTextureResourceViewAddress();

private:
    void Initialize1x1ColorTexture(ID3D11Device* device, const DirectX::SimpleMath::Color& colorData, aiTextureType type);
    void InitializeColorTexture(ID3D11Device* device, const DirectX::SimpleMath::Color* colorData, UINT width, UINT height, aiTextureType type);
    Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
    aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};
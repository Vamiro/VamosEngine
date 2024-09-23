#include "Texture.h"



Texture::Texture(ID3D11Device* device, const DirectX::SimpleMath::Color& color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, DirectX::SimpleMath::Color(100, 100, 100), type);
		}
		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, DirectX::SimpleMath::Color(100, 100, 100), type);
		}
		return;
	}
}

Texture::Texture(ID3D11Device* device, const DirectX::SimpleMath::Color* colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device,pData,size,this->texture.GetAddressOf(),this->textureView.GetAddressOf());
	ErrorLogger::Log(hr,"Failed to create texture from memory.");

}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const DirectX::SimpleMath::Color& colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const DirectX::SimpleMath::Color* colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;

	// Измените формат на R32G32B32A32_FLOAT для нормализованных значений
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R32G32B32A32_FLOAT, width, height);
	ID3D11Texture2D* p2DTexture = nullptr;

	// Создайте буфер для хранения данных цвета
	std::vector<DirectX::SimpleMath::Color> colorValues(width * height);

	// Копируйте данные напрямую
	std::copy(colorData, colorData + (width * height), colorValues.begin());

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorValues.data();
	initialData.SysMemPitch = width * sizeof(DirectX::SimpleMath::Color);

	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	ErrorLogger::Log(hr, "Failed to initialize texture from color data.");
	texture = static_cast<ID3D11Texture2D*>(p2DTexture);

	// Создание Shader Resource View
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	ErrorLogger::Log(hr, "Failed to create shader resource view from texture generated from color data.");
}
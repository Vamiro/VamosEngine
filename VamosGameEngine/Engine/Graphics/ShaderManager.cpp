﻿// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "ShaderManager.h"

#include "Graphics.h"

ShaderManager::ShaderManager()
{

}

void ShaderManager::InitShader(ShaderData data)
{
	auto shaderPair = shaders.find(data.name);
	if (shaderPair != shaders.end())
	{
		return;
	}

	Shaders inst;
	std::cout << data.name << std::endl;
	if (data.type & VertexType) {
		ID3DBlob* vsBlob;
		ID3D11VertexShader* vertexShader;

		CompileShaderFromFile(data.name, nullptr, "VSMain", "vs_5_0", &vsBlob);
		_graphics->GetDevice()->CreateVertexShader(
			vsBlob->GetBufferPointer(),
			vsBlob->GetBufferSize(),
			nullptr, &inst.VShader);

		if (inputLayout == nullptr) {
			CreateInputLayoutDescFromVertexShaderSignature(vsBlob, _graphics->GetDevice().Get(), &inputLayout);
		}
		vsBlob->Release();

		std::cout << "init VS:" << std::endl;
	}

	if (data.type & PixelType) {
		ID3DBlob* psBlob;
		ID3D11PixelShader* pixelShader;


		CompileShaderFromFile(data.name, nullptr, "PSMain", "ps_5_0", &psBlob);
		_graphics->GetDevice()->CreatePixelShader(
			psBlob->GetBufferPointer(),
			psBlob->GetBufferSize(),
			nullptr, &inst.PShader);
		psBlob->Release();

		std::cout << "init PS:" << std::endl;
	}

	if (data.type & GeometryType) {
		ID3DBlob* gsBlob;
		ID3D11PixelShader* geometryShader;

		CompileShaderFromFile(data.name, nullptr, "GSMain", "gs_5_0", &gsBlob);
		_graphics->GetDevice()->CreateGeometryShader(
			gsBlob->GetBufferPointer(),
			gsBlob->GetBufferSize(),
			nullptr, &inst.GShader);
		gsBlob->Release();

		std::cout << "init GS:" << std::endl;
	}

	shaders[data.name]  = inst;

}

void ShaderManager::SetShader(ShaderData data)
{
	auto shaderPair = shaders.find(data.name);
	if (shaderPair == shaders.end())
	{
		return;
	}

	Shaders shader = shaders[data.name];
	if (data.type & VertexType) {
		_graphics->GetDeviceContext()->IASetInputLayout(inputLayout);
		_graphics->GetDeviceContext()->VSSetShader(shader.VShader, nullptr, 0);
	}
	if (data.type & PixelType) {
		_graphics->GetDeviceContext()->PSSetShader(shader.PShader, nullptr, 0);
	}

	if (data.type & GeometryType) {
		_graphics->GetDeviceContext()->GSSetShader(shader.GShader, nullptr, 0);
	}


}

void ShaderManager::CompileShaderFromFile(std::string shaderName, D3D_SHADER_MACRO shaderMacros[], LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobout)
{

	std::wstring wshaderName = std::wstring(shaderName.begin(), shaderName.end());
	ID3DBlob* errorBlob;
	auto res = D3DCompileFromFile(wshaderName.c_str(), shaderMacros, NULL, entryPoint, shaderModel, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, blobout, &errorBlob);
	ErrorLogger::Log(res, "Failed to compile shader");
}

HRESULT ShaderManager::CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
{
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
	{
		return S_FALSE;
	}


	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT32 i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		if (paramDesc.Mask == 1) {
			switch (paramDesc.ComponentType) {
			case D3D_REGISTER_COMPONENT_UINT32: elementDesc.Format = DXGI_FORMAT_R32_UINT; break;
			case D3D_REGISTER_COMPONENT_SINT32: elementDesc.Format = DXGI_FORMAT_R32_SINT; break;
			case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
			default:break;
			}
		}
		else
			if (paramDesc.Mask <= 3) {
				switch (paramDesc.ComponentType) {
				case D3D_REGISTER_COMPONENT_UINT32: elementDesc.Format = DXGI_FORMAT_R32G32_UINT; break;
				case D3D_REGISTER_COMPONENT_SINT32: elementDesc.Format = DXGI_FORMAT_R32G32_SINT; break;
				case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				default:break;
				}
			}
			else
				if (paramDesc.Mask <= 7) {
					switch (paramDesc.ComponentType) {
					case D3D_REGISTER_COMPONENT_UINT32: elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT; break;
					case D3D_REGISTER_COMPONENT_SINT32: elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT; break;
					case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
					default:break;
					}
				}

				else if (paramDesc.Mask <= 15) {
					switch (paramDesc.ComponentType) {
					case D3D_REGISTER_COMPONENT_UINT32: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT; break;
					case D3D_REGISTER_COMPONENT_SINT32: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT; break;
					case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
					default:break;
					}
				}

		inputLayoutDesc.push_back(elementDesc);
	}

	HRESULT hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

	pVertexShaderReflection->Release();
	return hr;

}
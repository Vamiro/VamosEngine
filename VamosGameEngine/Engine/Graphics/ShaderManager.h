#pragma once

#include <wrl/client.h>
#include <string>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <map>
#include <vector>


enum ShaderType
{
    VertexType = 1 << 0,
    PixelType = 1 << 1,
    GeometryType = 1 << 2,
    ComputeType = 1 << 3
};
DEFINE_ENUM_FLAG_OPERATORS(ShaderType)


struct ShaderData {
    std::string name;
    ShaderType type;
};

class Graphics;


struct Shaders {
    ID3D11VertexShader* VShader = nullptr;
    ID3D11PixelShader* PShader = nullptr;
    ID3D11GeometryShader* GShader = nullptr;
    ID3D11ComputeShader* CShader = nullptr;
};

class ShaderManager
{
public:
    ShaderManager();
    ShaderManager(Graphics* graphics) : _graphics(graphics)
    {
    };
    ~ShaderManager();
    void InitShader(ShaderData data);
    void SetShader(ShaderData data);
    void CompileShaderFromFile(std::string shaderName, D3D_SHADER_MACRO shaderMacros[], LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** blobout);
private:
    Graphics* _graphics;
    std::map<std::string, Shaders> shaders;

    ID3D11InputLayout* inputLayout = nullptr;

    HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout);

};
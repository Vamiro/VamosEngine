#pragma once
#include "model.h"
#include <DirectXMath.h>
#include "Engine/Components/Transform.h"

//TODO: Use simple math library and quaternions

using namespace DirectX;

class GameObject
{
public:
    GameObject();
    bool Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                    ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader);
    void Draw(const XMMATRIX& viewProjectionMatrix);
    void SetColor(const SimpleMath::Color& value) { color = value; }
    Transform transform;
private:
    SimpleMath::Color color;
    Model model;
};

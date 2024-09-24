#pragma once
#include "model.h"
#include <DirectXMath.h>

#include "Engine/Components/Transform.h"

//TODO: Use simple math library and quaternions

using namespace DirectX;

class GameObject
{
public:
    explicit GameObject(const std::string& name);

    bool Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                    ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader);

    void Draw(const XMMATRIX& viewProjectionMatrix);
    virtual void Update();

    [[nodiscard]] SimpleMath::Color GetColor() const { return color; };
    void SetColor(const SimpleMath::Color& value) { color = value; }

    Transform transform;
    std::string name;

protected:
    SimpleMath::Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    Model model;
};

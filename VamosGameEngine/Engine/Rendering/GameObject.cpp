#include "GameObject.h"

GameObject::GameObject(const std::string& name) : name(name), model()
{
}

bool GameObject::Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                            ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader)
{
    if (!model.Initialize(filePath, device.Get(), deviceContext.Get(), cb_vs_vertexshader, cb_ps_pixelshader))
    {
        return false;
    }

    this->transform.SetPosition(0.0f, 0.0f, 0.0f);
    this->transform.SetRotation(0.0f, 0.0f, 0.0f);
    this->transform.UpdateWorldMatrix();
    return true;
}

void GameObject::Draw(const XMMATRIX& viewProjectionMatrix)
{
    if(color != nullptr)
    {
        model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix, color);
    }
    else
    {
        model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix);
    }
}

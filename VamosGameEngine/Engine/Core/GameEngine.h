#pragma once
#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_dx11.h>
#include <ImGUI/imgui_impl_win32.h>

#include "Engine.h"
#include "Engine/Utilities/Timer.h"

class GameEngine : public Engine {
public:
    bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height) override;
    bool ProcessMessages() override;
    void RenderFrame() override;

protected:
    Timer timer;

    ConstantBuffer<CB_VS_VertexShader> cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader> cb_ps_pixelshader;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;

    std::vector<GameObject*> gameObjects;
    int currentGameObj = 0;
    BoundingSphere boundingSphere;
};
#pragma once

#include "ImGUI/imgui.h"
#include "Component.h"
#include "../Rendering/Mesh.h"

class Model : public Component
{
public:
    explicit Model(Object& parent, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                    ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader);
    ~Model() override;

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void Draw(const DirectX::SimpleMath::Matrix& worldMatrix, const DirectX::SimpleMath::Matrix& viewProjectionMatrix);

    void SetModelDirectory(const std::string& filePath);
    [[nodiscard]] DirectX::SimpleMath::Color GetColor() const { return color; }
    void SetColor(const DirectX::SimpleMath::Color& value) { color = value; }

private:
    DirectX::SimpleMath::Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<Mesh> meshes;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

    ConstantBuffer<CB_VS_VertexShader>* cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader>* cb_ps_pixelshader;
    std::string directory = "";

    bool LoadModel();
    void ProcessNode(aiNode* node, const aiScene* Scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* Scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
    TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index,
                                                   aiTextureType textureType);
    int GetTextureIndex(aiString* pStr);
};

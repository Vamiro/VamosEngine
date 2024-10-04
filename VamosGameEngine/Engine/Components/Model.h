#pragma once

#include <ImGUI/imgui.h>

#include "Component.h"
#include "Engine/Rendering/Mesh.h"
#include "ModelBuffer.h"

class Model : public Component
{
public:
    explicit Model(GameObject& parent, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                    ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader);
    ~Model() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    void RenderGUI() override;
    void Draw(const DirectX::SimpleMath::Matrix& worldMatrix, const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projectionMatrix);

    void SetModelPath(const std::string& filePath);
    [[nodiscard]] std::string GetModelPath() const { return filePath; }
    [[nodiscard]] DirectX::SimpleMath::Color GetColor() const { return color; }
    void SetColor(const DirectX::SimpleMath::Color& value) { color = value; }

    void SetTexture(const std::string& texturePath);

private:
    DirectX::SimpleMath::Color color = {1.0f, 1.0f, 1.0f, 1.0f};

    int modelIndex = -1;
    std::vector<Texture> textures;

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

    ConstantBuffer<CB_VS_VertexShader>* cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader>* cb_ps_pixelshader;
    std::string directory = "";
    std::string filePath = "";

    bool LoadModel();
    static bool modelsLoaded;
    static void LoadAllModels(const std::string& filePath);

    void ProcessNode(aiNode* node, const aiScene* Scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* Scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
    TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index,
                                                   aiTextureType textureType);
    int GetTextureIndex(aiString* pStr);

};

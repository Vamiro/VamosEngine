#pragma once
#include "Mesh.h"


class Model
{
public:
    bool Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                    ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader, ConstantBuffer<CB_PS_PixelShader>& cb_ps_pixelshader);
    void Draw(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewProjectionMatrix,
              DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));

private:
    std::vector<Mesh> meshes;
    bool LoadModel(std::string filePath);
    void ProcessNode(aiNode* node, const aiScene* Scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* Scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene);
    TextureStorageType DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index,
                                                   aiTextureType textureType);
    int GetTextureIndex(aiString* pStr);

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

    ConstantBuffer<CB_VS_VertexShader>* cb_vs_vertexshader;
    ConstantBuffer<CB_PS_PixelShader>* cb_ps_pixelshader;
    std::string directory = "";
};

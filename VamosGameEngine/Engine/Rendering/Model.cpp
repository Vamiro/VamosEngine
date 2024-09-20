#include "Model.h"

#include <utility>

bool Model::Initialize(const std::string& filePath, const Microsoft::WRL::ComPtr<ID3D11Device>& device, const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext,
                       ConstantBuffer<CB_VS_VertexShader>& cb_vs_vertexshader)
{
    this->device = device.Get();
    this->deviceContext = deviceContext.Get();
    this->cb_vs_vertexshader = &cb_vs_vertexshader;

    try
    {
        if (!this->LoadModel(filePath))
        {
        }
    }
    catch (COMException& exception)
    {
        ErrorLogger::Log(exception);
        return false;
    }
    return true;
}

void Model::Draw(const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewProjectionMatrix)
{
    //Update Constant buffer with WVP Matrix
    this->cb_vs_vertexshader->data.mat = worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix
    this->cb_vs_vertexshader->data.mat = XMMatrixTranspose(this->cb_vs_vertexshader->data.mat);
    this->cb_vs_vertexshader->ApplyChanges(0);
    this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());


    for (DWORD i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw();
    }
}


bool Model::LoadModel(const std::string filePath)
{
    this->directory = StringHelper::GetDirectoryFromPath(filePath);
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
    if (pScene == nullptr)
    {
        return false;
    }
    this->ProcessNode(pScene->mRootNode, pScene);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for (UINT i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(this->ProcessMesh(mesh, scene));
    }

    for (UINT i = 0; i < node->mNumChildren; i++)
    {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType,
                                                 const aiScene* pScene)
{
    std::vector<Texture> materialTextures;
    auto storetype = TextureStorageType::Invalid;
    unsigned int textureCount = pMaterial->GetTextureCount(textureType);

    if (textureCount == 0) //If there are no textures
    {
        storetype = TextureStorageType::None;
        aiColor3D aiColor(0.0f, 0.0f, 0.0f);
        switch (textureType)
        {
        case aiTextureType_DIFFUSE:
            pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
            if (aiColor.IsBlack()) //If color = black, just use grey
            {
                materialTextures.push_back(
                    Texture(this->device.Get(), DirectX::SimpleMath::Color(100, 100, 100), textureType));
                return materialTextures;
            }
            materialTextures.push_back(Texture(this->device.Get(),
                                               DirectX::SimpleMath::Color(
                                                   aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
            return materialTextures;
        }
    }
    else
    {
        for (UINT i = 0; i < textureCount; i++)
        {
            aiString path;
            pMaterial->GetTexture(textureType, i, &path);
            TextureStorageType storageType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
            switch (storageType)
            {
            case TextureStorageType::Disk:
                {
                    std::string fileName = this->directory + "\\" + path.C_Str();
                    Texture diskTexture(this->device.Get(), fileName, textureType);
                    materialTextures.push_back(diskTexture);
                    break;
                }
            case TextureStorageType::EmbeddedCompressed:
                {
                    const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
                    Texture embeddedTexture(this->device.Get(), reinterpret_cast<uint8_t*>(pTexture->pcData),
                                            pTexture->mWidth, textureType);
                    materialTextures.push_back(embeddedTexture);
                    break;
                }
            case TextureStorageType::EmbeddedIndexCompressed:
                {
                    int index = GetTextureIndex(&path);
                    Texture embeddedIndexedTexture(this->device.Get(),
                                                   reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
                                                   pScene->mTextures[index]->mWidth,
                                                   textureType);
                    materialTextures.push_back(embeddedIndexedTexture);
                    break;
                }
            }
        }
    }
    if (materialTextures.size() == 0)
    {
        materialTextures.push_back(Texture(this->device.Get(), DirectX::SimpleMath::Color(250, 0, 0),
                                           aiTextureType_DIFFUSE));
    }
    return materialTextures;
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index,
                                                      aiTextureType textureType)
{
    if (pMat->GetTextureCount(textureType) == 0)
        return TextureStorageType::None;

    aiString path;
    pMat->GetTexture(textureType, index, &path);
    std::string texturePath = path.C_Str();
    //Check if texture is an embedded indexed texture by seeing if the file path is an index #
    if (texturePath[0] == '*')
    {
        if (pScene->mTextures[0]->mHeight == 0)
        {
            return TextureStorageType::EmbeddedIndexCompressed;
        }
        assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
        return TextureStorageType::EmbeddedIndexNonCompressed;
    }
    //Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
    if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
    {
        if (pTex->mHeight == 0)
        {
            return TextureStorageType::EmbeddedCompressed;
        }
        assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
        return TextureStorageType::EmbeddedNonCompressed;
    }
    //Lastly check if texture is a filepath by checking for period before extension name
    if (texturePath.find('.') != std::string::npos)
    {
        return TextureStorageType::Disk;
    }

    return TextureStorageType::None; // No texture exists
}

int Model::GetTextureIndex(aiString* pStr)
{
    assert(pStr->length>=2);
    return atoi(&pStr->C_Str()[1]);
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* Scene)
{
    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;

    for (UINT i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        if (mesh->mTextureCoords[0])
        {
            vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    //Get indices
    for (UINT i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for (UINT j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    std::vector<Texture> textures;
    aiMaterial* material = Scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType_DIFFUSE, Scene);
    textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

    return Mesh(this->device, this->deviceContext, vertices, indices, textures);
}

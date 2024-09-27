#include "GameObject.h"

#include "Engine/Components/Model.h"

void GameObject::Start()
{
    for (auto component : components)
    {
        component->Start();
    }
}

void GameObject::Update(float deltaTime)
{
    for (auto component : components)
    {
        component->Update();
    }
}

void GameObject::Render(const SimpleMath::Matrix& viewProjectionMatrix)
{
    Model* mModel = GetComponent<Model>();
    if(mModel != nullptr)
    {
        mModel->Draw(this->transform->GetWorldMatrix(), viewProjectionMatrix);
    }
}

void GameObject::Destroy()
{
    delete transform;
}

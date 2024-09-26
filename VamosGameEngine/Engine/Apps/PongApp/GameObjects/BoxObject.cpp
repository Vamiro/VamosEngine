
#include "BoxObject.h"

BoxObject::BoxObject(const std::string& name) : GameObject(name), boxCollider(nullptr), model(nullptr)
{
    boxCollider = new BoxCollider(*this, SimpleMath::Vector3(1.0f, 1.0f, 1.0f));
    components.push_back(boxCollider);

    model = new Model(*this);
    components.push_back(model);
}

void BoxObject::Destroy()
{
}

void BoxObject::Update(float deltaTime)
{
    GameObject::Update(deltaTime);
}

void BoxObject::Render(const SimpleMath::Matrix& viewProjectionMatrix)
{
    model->Draw(this->transform->GetWorldMatrix(), viewProjectionMatrix);
}


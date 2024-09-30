#include "GameObject.h"

#include "Engine/Components/Component.h"
#include "Engine/Components/Transform.h"
#include "Engine/Components/Model.h"
#include "Engine/Components/ColliderComponent.h"

void GameObject::Start()
{
    for (auto component : _components)
    {
        component->Start();
    }
}

void GameObject::Update(float deltaTime)
{
    for (auto component : _components)
    {
        component->Update();
    }
}

GameObject::GameObject(const std::string& name, bool visible): name(name), _isVisible(visible)
{
    transform = new Transform(*this);
    _components.push_back(transform);
}

void GameObject::Render(const SimpleMath::Matrix& viewProjectionMatrix)
{
    for (auto child : _children)
    {
        child->transform->SetParentWorldMatrix(this->transform->GetWorldMatrix());
    }
    Model* mModel = GetComponent<Model>();
    if(mModel != nullptr)
    {
        mModel->Draw(this->transform->GetWorldMatrix(), viewProjectionMatrix);
    }

}

void GameObject::RenderComponentsGUI()
{

    ImGui::Checkbox("Visible", &_isVisible);
    for (const auto component : _components)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 5), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * 5));
        ImGui::BeginChild(component->GetName(), ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
        ImGui::SeparatorText(component->GetName());
        component->RenderGUI();
        ImGui::EndChild();
    }
}

void GameObject::Destroy()
{
    delete transform;
}

void GameObject::SetParent(GameObject* parent)
{
    if (parent == nullptr)
    {
        return;
    }

    auto parentWorldMatrix = parent->transform->GetWorldMatrix();
    auto currentWorldMatrix = transform->GetWorldMatrix();
    auto localMatrix = currentWorldMatrix * parentWorldMatrix.Invert();

    transform->SetLocalMatrix(localMatrix);
    _parent = parent;
    _parent->AddChild(this);

}

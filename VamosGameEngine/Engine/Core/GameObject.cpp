#include "GameObject.h"

#include "Engine/Components/Transform.h"
#include "Engine/Components/Model.h"

int GameObject::nextId = 0;

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
        component->Update(deltaTime);
    }
}

GameObject::GameObject(const std::string& name, const bool visible): name(name), id(nextId++), _isVisible(visible)
{
    transform = new Transform(*this);
    _components.push_back(transform);
}

void GameObject::Render(const DirectX::SimpleMath::Matrix& viewMatrix, const DirectX::SimpleMath::Matrix& projectionMatrix,
    DirectX::SimpleMath::Vector3 lightDirection)
{
    Model* mModel = GetComponent<Model>();
    if(mModel != nullptr)
    {
        mModel->Draw(this->transform->GetWorldMatrix(), viewMatrix, projectionMatrix, lightDirection);
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

    _parent = parent;
    _parent->AddChild(this);
    transform->SetParent(parent->transform);
}

void GameObject::DeleteComponent(Component* component)
{
    component->Destroy();
    std::erase(_components, component);
}

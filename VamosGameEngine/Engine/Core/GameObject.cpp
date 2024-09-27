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

void GameObject::RenderComponentsGUI()
{

    ImGui::Checkbox("Visible", &_isVisible);
    for (const auto component : components)
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

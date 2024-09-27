#include "Object.h"

Object::Object(const std::string& name) : name(name)
{
    transform = new Transform(*this);
    components.push_back(transform);
}

void Object::RenderComponentsGUI() const
{
    for (const auto component : components)
    {
        ImGui::SetNextWindowSizeConstraints(ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 5), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * 5));
        ImGui::BeginChild(component->GetName(), ImVec2(0.0f, 0.0f), true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
        ImGui::SeparatorText(component->GetName());
        component->RenderGUI();
        ImGui::EndChild();
    }
}

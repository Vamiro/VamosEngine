#include "ImGuiHelper.h"


bool ImGuiHelper::Vector3GUI(const char* label, DirectX::SimpleMath::Vector3& vec, float speed, float min, float max)
{
    float values[3] = {vec.x, vec.y, vec.z};
    ImGui::DragFloat3(label, values, 0.1f);
    if (ImGui::IsItemActive()) {
        vec = DirectX::SimpleMath::Vector3(values[0], values[1], values[2]);
    }
    return ImGui::IsItemActive();
}

bool ImGuiHelper::FloatGUI(const char* label, float& value, float speed, float min, float max)
{
    ImGui::DragFloat(label, &value, speed, min, max);
    return ImGui::IsItemActive();
}
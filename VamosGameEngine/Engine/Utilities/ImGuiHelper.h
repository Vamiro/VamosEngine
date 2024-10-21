#pragma once
#include <ImGUI/imgui.h>
#include <SimpleMath.h>
#include <limits>

class ImGuiHelper {
public:
    static bool Vector3GUI(const char* label, DirectX::SimpleMath::Vector3& vec, float speed = 0.1f,
                       float min = std::numeric_limits<float>::lowest(), float max = -std::numeric_limits<float>::lowest());
    static bool FloatGUI(const char* label, float& value, float speed = 0.1f,
                           float min = std::numeric_limits<float>::lowest(), float max = -std::numeric_limits<float>::lowest());
};

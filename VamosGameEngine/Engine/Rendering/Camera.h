#pragma once
#include "SimpleMath.h"
#include "Engine/Components/Transform.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Core/Object.h"

using namespace DirectX;

enum ProjectionType {
    PERSPECTIVE = 1 << 0,
    ORTHOGRAPHIC = 1 << 1,
};
DEFINE_ENUM_FLAG_OPERATORS(ProjectionType)

class Camera : public GameObject
{
public:
    Camera();
    void Destroy() override;
    void SetProjectionValues(float FOV, float aspectRatio, float nearZ, float farZ, ProjectionType type);

    [[nodiscard]] const SimpleMath::Matrix& GetViewMatrix() const;
    [[nodiscard]] const SimpleMath::Matrix& GetProjectionMatrix() const;
    void UpdateViewMatrix();
    [[nodiscard]] const SimpleMath::Vector3& GetCamTarget() const { return this->camTarget; }

private:
    SimpleMath::Vector3 camTarget{};
    SimpleMath::Matrix viewMatrix{};
    SimpleMath::Matrix projectionMatrix{};
};

#pragma once
#include "Engine/Core/GameObject.h"

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
    void UpdateProjectionMatrix();
    void SetProjectionValues(float FOV, float aspectRatio, float nearZ, float farZ, ProjectionType type);
    void SetProjectionValues(float aspectRatio);

    [[nodiscard]] const SimpleMath::Matrix& GetViewMatrix() const;
    [[nodiscard]] const SimpleMath::Matrix& GetProjectionMatrix() const;
    void UpdateViewMatrix();
    [[nodiscard]] const SimpleMath::Vector3& GetCamTarget() const { return this->camTarget; }

private:
    SimpleMath::Vector3 camTarget{};
    SimpleMath::Matrix viewMatrix{};
    SimpleMath::Matrix projectionMatrix{};
    float _fovRadians;
    float _fovDegrees;
    float _aspectRatio;
    float _nearZ;
    float _farZ;
    ProjectionType _type;
};

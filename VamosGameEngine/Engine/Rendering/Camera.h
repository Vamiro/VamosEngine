#pragma once
#include <DirectXMath.h>
#include <wrl.h>

#include "Engine/Components/Transform.h"

//TODO: Use simple math library and quaternions for camera

using namespace DirectX;

enum ProjectionType {
    PERSPECTIVE = 1 << 0,
    ORTHOGRAPHIC = 1 << 1,
};
DEFINE_ENUM_FLAG_OPERATORS(ProjectionType)

class Camera
{
public:
    Camera();
    void SetProjectionValues(float FOV, float aspectRatio, float nearZ, float farZ, ProjectionType type);

    [[nodiscard]] const XMMATRIX& GetViewMatrix() const;
    [[nodiscard]] const XMMATRIX& GetProjectionMatrix() const;
    void UpdateViewMatrix();
    [[nodiscard]] const XMVECTOR& GetCamTarget() const { return this->camTarget; }

    Transform transform;

private:
    XMVECTOR camTarget{};
    XMMATRIX viewMatrix{};
    XMMATRIX projectionMatrix{};
};

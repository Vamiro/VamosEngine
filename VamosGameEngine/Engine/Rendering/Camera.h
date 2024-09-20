#pragma once
#include <DirectXMath.h>

#include "Engine/Components/Transform.h"

//TODO: Use simple math library and quaternions for camera

using namespace DirectX;

class Camera
{
public:
    Camera();
    void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

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

#include "Camera.h"
#include "Engine/Components/Transform.h"

Camera::Camera() : GameObject("Camera", false)
{
    this->UpdateViewMatrix();
}

void Camera::Destroy()
{
    delete transform;
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ, ProjectionType type)
{
    float fovRadians = XMConvertToRadians(fovDegrees);

    if (type & PERSPECTIVE)
    {
        this->projectionMatrix = SimpleMath::Matrix::CreatePerspectiveFieldOfView(fovRadians, aspectRatio, nearZ, farZ);
    }
    else if (type & ORTHOGRAPHIC)
    {
        this->projectionMatrix = SimpleMath::Matrix::CreateOrthographic(fovDegrees, fovDegrees / aspectRatio, nearZ, farZ);
    }
}

const SimpleMath::Matrix& Camera::GetViewMatrix() const
{
    return this->viewMatrix;
}

const SimpleMath::Matrix& Camera::GetProjectionMatrix() const
{
    return this->projectionMatrix;
}

void Camera::UpdateViewMatrix()
{
    this->transform->UpdateWorldMatrix();

    // Вычисляем матрицу вращения камеры на основе кватерниона
    SimpleMath::Matrix camRotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(this->transform->GetGlobalRotation());

    // Вычисляем целевой вектор камеры
    camTarget = SimpleMath::Vector3::Transform(this->transform->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
    camTarget += this->transform->GetGlobalPosition();

    // Обновляем направление "вверх" камеры
    SimpleMath::Vector3 upDir = SimpleMath::Vector3::Transform(this->transform->DEFAULT_UP_VECTOR, camRotationMatrix);

    // Перестраиваем матрицу вида
    this->viewMatrix = SimpleMath::Matrix::CreateLookAt(this->transform->GetGlobalPosition(), camTarget, upDir);
}

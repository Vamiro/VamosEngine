#include "Camera.h"

Camera::Camera() : Object("Camera")
{
    this->UpdateViewMatrix();
}

void Camera::Destroy()
{
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ, ProjectionType type)
{
    float fovRadians = DirectX::XMConvertToRadians(fovDegrees);

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
    SimpleMath::Matrix camRotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(this->transform->GetRotationQuaternion());

    // Вычисляем целевой вектор камеры
    camTarget = SimpleMath::Vector3::Transform(this->transform->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
    camTarget += this->transform->GetPositionVector();

    // Обновляем направление "вверх" камеры
    SimpleMath::Vector3 upDir = SimpleMath::Vector3::Transform(this->transform->DEFAULT_UP_VECTOR, camRotationMatrix);

    // Перестраиваем матрицу вида
    this->viewMatrix = SimpleMath::Matrix::CreateLookAt(this->transform->GetPositionVector(), camTarget, upDir);
}

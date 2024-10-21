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

void Camera::UpdateProjectionMatrix()
{
    if (_type & PERSPECTIVE)
    {
        this->projectionMatrix = SimpleMath::Matrix::CreatePerspectiveFieldOfView(_fovRadians, _aspectRatio, _nearZ, _farZ);
    }
    else if (_type & ORTHOGRAPHIC)
    {
        this->projectionMatrix = SimpleMath::Matrix::CreateOrthographic(_fovDegrees, _fovDegrees / _aspectRatio, _nearZ, _farZ);
    }
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ, ProjectionType type)
{
    _fovRadians = XMConvertToRadians(fovDegrees);
    _fovDegrees = fovDegrees;
    _aspectRatio = aspectRatio;
    _nearZ = nearZ;
    _farZ = farZ;
    _type = type;

    UpdateProjectionMatrix();
}

void Camera::SetProjectionValues(float aspectRatio)
{
    _aspectRatio = aspectRatio;
    UpdateProjectionMatrix();
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

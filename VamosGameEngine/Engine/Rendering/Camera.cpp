#include "Camera.h"

#include "SimpleMath.h"

Camera::Camera()
{
    this->UpdateViewMatrix();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ, ProjectionType type)
{
    float fovRadians = (fovDegrees / 360.0f) * XM_2PI;

    if (type & PERSPECTIVE)
    {
        this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
    } else if (type & ORTHOGRAPHIC)
    {
        this->projectionMatrix = XMMatrixOrthographicLH(fovDegrees, fovDegrees/aspectRatio, nearZ, farZ);
    }
}

const XMMATRIX& Camera::GetViewMatrix() const
{
    return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
    return this->projectionMatrix;
}

void Camera::UpdateViewMatrix() //Updates view matrix and also updates the movement vectors
{
    this->transform.UpdateWorldMatrix();
    //Calculate camera rotation matrix
    XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->transform.GetRotationFloat3().x, this->transform.GetRotationFloat3().y, this->transform.GetRotationFloat3().z);
    //Calculate unit vector of cam target based off camera forward value transformed by cam rotation matrix
    camTarget = XMVector3TransformCoord(this->transform.DEFAULT_FORWARD_VECTOR, camRotationMatrix);
    //Adjust cam target to be offset by the camera's current position
    camTarget += this->transform.GetPositionVector();
    //Calculate up direction based on current rotation
    XMVECTOR upDir = XMVector3TransformCoord(this->transform.DEFAULT_UP_VECTOR, camRotationMatrix);
    //Rebuild view matrix
    this->viewMatrix = XMMatrixLookAtLH(this->transform.GetPositionVector(), camTarget, upDir);

    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->transform.GetRotationFloat3().x, this->transform.GetRotationFloat3().y, 0.0f);
}

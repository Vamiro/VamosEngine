#include "Transform.h"

#include "SimpleMath.h"
#include "ImGUI/imgui.h"


Transform::Transform()
{
    this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->posVector = XMLoadFloat3(&this->pos);

    this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->rotVector = XMLoadFloat3(&this->rot);

    this->scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    this->scaleVector = XMLoadFloat3(&this->scale);
}

void Transform::UpdateWorldMatrix()
{
    XMMATRIX scaleMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
    XMMATRIX translationMatrix = XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);

    // Объединяем матрицы: масштаб, потом вращение, потом перевод
    this->worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    // Обновляем векторы направления
    this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, rotationMatrix);
    this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, rotationMatrix);
    this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, rotationMatrix);
    this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, rotationMatrix);
    this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, rotationMatrix);
}

XMVECTOR Transform::DegreesToRadians(const float x, const float y, const float z)
{
    XMVECTOR degrees = XMVectorSet(x, y, z, 0.0f);
    degrees = degrees * XM_PI / 180;
    return degrees;
}

void Transform::RenderGUI()
{
    
}

const XMVECTOR& Transform::GetPositionVector() const
{
    return this->posVector;
}

const XMFLOAT3& Transform::GetPositionFloat3() const
{
    return this->pos;
}

const XMVECTOR& Transform::GetScaleVector() const
{
    return this->scaleVector;
}

const XMFLOAT3& Transform::GetScaleFloat3() const
{
    return this->scale;
}

const XMVECTOR& Transform::GetRotationVector() const
{
    return this->rotVector;
}

const XMFLOAT3& Transform::GetRotationFloat3() const
{
    return this->rot;
}

void Transform::SetPosition(const XMVECTOR& pos)
{
    XMStoreFloat3(&this->pos, pos);
    this->posVector = pos;
    this->UpdateWorldMatrix();
}

void Transform::SetPosition(const XMFLOAT3& pos)
{
    this->pos = pos;
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateWorldMatrix();
}

void Transform::SetPosition(float x, float y, float z)
{
    this->pos = XMFLOAT3(x, y, z);
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateWorldMatrix();
}

void Transform::AdjustPosition(const XMVECTOR& pos)
{
    this->posVector += pos;
    XMStoreFloat3(&this->pos, this->posVector);
    this->UpdateWorldMatrix();
}

void Transform::AdjustPosition(const XMFLOAT3& pos)
{
    this->pos.x += pos.y;
    this->pos.y += pos.y;
    this->pos.z += pos.z;
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateWorldMatrix();
}

void Transform::AdjustPosition(float x, float y, float z)
{
    this->pos.x += x;
    this->pos.y += y;
    this->pos.z += z;
    this->posVector = XMLoadFloat3(&this->pos);
    this->UpdateWorldMatrix();
}

void Transform::SetScale(const XMVECTOR& scale)
{
    XMStoreFloat3(&this->scale, scale);
    this->scaleVector = scale;
    this->UpdateWorldMatrix();
}

void Transform::SetScale(const XMFLOAT3& scale)
{
    this->scale = scale;
    this->scaleVector = XMLoadFloat3(&this->scale);
    this->UpdateWorldMatrix();
}

void Transform::SetScale(float x, float y, float z)
{
    this->scale = XMFLOAT3(x, y, z);
    this->scaleVector = XMLoadFloat3(&this->scale);
    this->UpdateWorldMatrix();
}

void Transform::AdjustScale(const XMVECTOR& scale)
{
    this->scaleVector += scale;
    XMStoreFloat3(&this->scale, this->scaleVector);
    this->UpdateWorldMatrix();
}

void Transform::AdjustScale(const XMFLOAT3& scale)
{
    this->scale.x += scale.x;
    this->scale.y += scale.y;
    this->scale.z += scale.z;
    this->scaleVector = XMLoadFloat3(&this->scale);
    this->UpdateWorldMatrix();
}

void Transform::AdjustScale(float x, float y, float z)
{
    this->scale.x += x;
    this->scale.y += y;
    this->scale.z += z;
    this->scaleVector = XMLoadFloat3(&this->scale);
    this->UpdateWorldMatrix();
}

void Transform::SetRotation(const XMVECTOR& rot)
{
    this->rotVector = rot;
    XMStoreFloat3(&this->rot, rot);
    this->UpdateWorldMatrix();
}

void Transform::SetRotation(const XMFLOAT3& rot)
{
    this->rot = rot;
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateWorldMatrix();
}

void Transform::SetRotation(float x, float y, float z)
{
    this->rot = XMFLOAT3(x, y, z);
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateWorldMatrix();
}

void Transform::AdjustRotation(const XMVECTOR& rot)
{
    this->rotVector += rot;
    XMStoreFloat3(&this->rot, this->rotVector);
    this->UpdateWorldMatrix();
}

void Transform::AdjustRotation(const XMFLOAT3& rot)
{
    this->rot.x += rot.x;
    this->rot.y += rot.y;
    this->rot.z += rot.z;
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateWorldMatrix();
}

void Transform::AdjustRotation(float x, float y, float z)
{
    this->rot.x += x;
    this->rot.y += y;
    this->rot.z += z;
    this->rotVector = XMLoadFloat3(&this->rot);
    this->UpdateWorldMatrix();
}

void Transform::SetLookAtPos(XMFLOAT3 lookAtPos)
{
    //Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
    if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
        return;

    lookAtPos.x = this->pos.x - lookAtPos.x;
    lookAtPos.y = this->pos.y - lookAtPos.y;
    lookAtPos.z = this->pos.z - lookAtPos.z;

    float pitch = 0.0f;
    if (lookAtPos.y != 0.0f)
    {
        const float distance = sqrt(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
        pitch = atan(lookAtPos.y / distance);
    }

    float yaw = 0.0f;
    if (lookAtPos.x != 0.0f)
    {
        yaw = atan(lookAtPos.x / lookAtPos.z);
    }
    if (lookAtPos.z > 0)
        yaw += XM_PI;

    this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Transform::GetForwardVector()
{
    return this->vec_forward;
}

const XMVECTOR& Transform::GetRightVector()
{
    return this->vec_right;
}

const XMVECTOR& Transform::GetBackwardVector()
{
    return this->vec_backward;
}

const XMVECTOR& Transform::GetLeftVector()
{
    return this->vec_left;
}

const XMVECTOR& Transform::GetUpVector()
{
    return this->vec_up;
}

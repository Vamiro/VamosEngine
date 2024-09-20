#include "Transform.h"


Transform::Transform()
{
    this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->posVector = XMLoadFloat3(&this->pos);
    this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    this->rotVector = XMLoadFloat3(&this->rot);
}

void Transform::UpdateWorldMatrix()
{
    this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(
        this->pos.x, this->pos.y, this->pos.z);
    XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
    this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
    this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
    this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
    this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
    this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrix);
}

const XMVECTOR& Transform::GetPositionVector() const
{
    return this->posVector;
}

const XMFLOAT3& Transform::GetPositionFloat3() const
{
    return this->pos;
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

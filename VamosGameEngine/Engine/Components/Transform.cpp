﻿#include "Transform.h"

#include <iostream>

#include "ColliderComponent.h"
#include "Engine/Core/Object.h"

Transform::Transform(Object& parent) : Component(parent, "Transform")
{
}

Transform::~Transform()
{
}

void Transform::Start()
{
}

void Transform::Update()
{
}

void Transform::RenderGUI()
{
    float pos[3] = {position.x, position.y, position.z};
    float euler[3] = {eulerAngles.x, eulerAngles.y, eulerAngles.z};
    float scale[3] = {this->scale.x, this->scale.y, this->scale.z};

    ImGui::DragFloat3("Position", pos, 0.1f);
    bool it = ImGui::IsItemActive();
    if(ImGui::IsItemActive())
        SetPosition(SimpleMath::Vector3(pos[0], pos[1], pos[2]));

    ImGui::DragFloat3("Euler", euler, 0.1f);
    it = it || ImGui::IsItemActive();
    if(ImGui::IsItemActive())
        SetEulerRotate(SimpleMath::Vector3(euler[0], euler[1], euler[2]));

    ImGui::DragFloat3("Scale", scale, 0.1f, 0.1f, 10000.0f);
    if(ImGui::IsItemActive())
        SetScale(SimpleMath::Vector3(scale[0], scale[1], scale[2]));


    if (auto* cc = this->parent->GetComponent<ColliderComponent>()) {
        if (ImGui::IsMouseDragging(0) && it) {
            cc->SetActivation(false); // Deactivate physics
            cc->SetPositionAndRotation(this->position, this->rotation);
        } else
        {
            cc->SetActivation(true); // Activate physics
        }
    }

}

void Transform::UpdateWorldMatrix()
{
    SimpleMath::Matrix scaleMatrix = SimpleMath::Matrix::CreateScale(this->scale);
    SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(this->rotation);
    SimpleMath::Matrix translationMatrix = SimpleMath::Matrix::CreateTranslation(this->position);

    // Объединяем матрицы: масштаб, потом вращение, потом перевод
    this->worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    // Обновляем векторы направления
    this->vec_forward = SimpleMath::Vector3::Transform(DEFAULT_FORWARD_VECTOR, rotationMatrix);
    this->vec_backward = SimpleMath::Vector3::Transform(DEFAULT_BACKWARD_VECTOR, rotationMatrix);
    this->vec_left = SimpleMath::Vector3::Transform(DEFAULT_LEFT_VECTOR, rotationMatrix);
    this->vec_right = SimpleMath::Vector3::Transform(DEFAULT_RIGHT_VECTOR, rotationMatrix);
    this->vec_up = SimpleMath::Vector3::Transform(DEFAULT_UP_VECTOR, rotationMatrix);
    this->vec_down = SimpleMath::Vector3::Transform(DEFAULT_DOWN_VECTOR, rotationMatrix);


}

const SimpleMath::Vector3& Transform::GetPositionVector() const
{
    return this->position;
}

const SimpleMath::Vector3& Transform::GetScaleVector() const
{
    return this->scale;
}

const SimpleMath::Quaternion& Transform::GetRotationQuaternion() const
{
    return this->rotation;
}

void Transform::SetPosition(const SimpleMath::Vector3& pos)
{
    this->position = pos;
    this->hasChanges = true;
    this->UpdateWorldMatrix();
}

void Transform::AdjustPosition(const SimpleMath::Vector3& pos)
{
    SetPosition(this->position + pos);
}

void Transform::SetScale(const SimpleMath::Vector3& scale)
{
    this->scale = scale;
    this->hasChanges = true;
    this->UpdateWorldMatrix();
}

void Transform::AdjustScale(const SimpleMath::Vector3& scale)
{
    SetScale(this->scale + scale);
}

void Transform::SetRotation(SimpleMath::Quaternion quaternion)
{
    eulerAngles = quaternion.ToEuler();
    rotation = quaternion;
    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::SetEulerRotate(const SimpleMath::Vector3& eulerAngle)
{
    eulerAngles = eulerAngle;
    rotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(Radians(eulerAngle.x), Radians(eulerAngle.y), Radians(eulerAngle.z));
    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::SetRadianRotate(const SimpleMath::Vector3& radians)
{
    eulerAngles = SimpleMath::Vector3(Degrees(radians.x), Degrees(radians.y), Degrees(radians.z));
    rotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(radians.x,radians.y,radians.z);
    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::AdjustRotation(const SimpleMath::Vector3& eulerAngle)
{
    SetEulerRotate(eulerAngles + eulerAngle);
}

void Transform::AdjustRotation(float roll, float pitch, float yaw)
{
    SetEulerRotate(eulerAngles + SimpleMath::Vector3(roll,pitch,yaw));
}

void Transform::SetLookAtPos(const SimpleMath::Vector3& lookAtPos)
{
    if (lookAtPos == this->position) return;

    SimpleMath::Vector3 direction = this->position - lookAtPos;
    direction.Normalize();

    SimpleMath::Quaternion lookAtQuat = SimpleMath::Quaternion::CreateFromRotationMatrix(SimpleMath::Matrix::CreateLookAt(this->position, lookAtPos, DEFAULT_UP_VECTOR));
    this->hasChanges = true;
    this->SetRotation(lookAtQuat);
}

const SimpleMath::Vector3& Transform::GetForwardVector()
{
    return this->vec_forward;
}

const SimpleMath::Vector3& Transform::GetRightVector()
{
    return this->vec_right;
}

const SimpleMath::Vector3& Transform::GetBackwardVector()
{
    return this->vec_backward;
}

const SimpleMath::Vector3& Transform::GetLeftVector()
{
    return this->vec_left;
}

const SimpleMath::Vector3& Transform::GetUpVector()
{
    return this->vec_up;
}

const SimpleMath::Vector3& Transform::GetDownVector()
{
    return this->vec_down;
}

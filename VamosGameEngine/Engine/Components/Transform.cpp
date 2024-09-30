#include "Transform.h"
#include "ColliderComponent.h"
#include "Engine/Core/GameObject.h"

Transform::Transform(GameObject& parent) : Component(parent, "Transform")
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

void Transform::Destroy()
{
}

void Transform::RenderGUI()
{
    float pos[3] = {position.x, position.y, position.z};
    float euler[3] = {XMConvertToDegrees(eulerAngles.x), XMConvertToDegrees(eulerAngles.y), XMConvertToDegrees(eulerAngles.z)};
    float scale[3] = {this->scale.x, this->scale.y, this->scale.z};

    ImGui::DragFloat3("Position", pos, 0.1f);
    bool it = ImGui::IsItemActive();
    if(ImGui::IsItemActive())
        SetPosition(SimpleMath::Vector3(pos[0], pos[1], pos[2]));

    ImGui::DragFloat3("Euler", euler, 0.1f);
    it = it || ImGui::IsItemActive();
    if(ImGui::IsItemActive())
        SetRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(euler[1]), XMConvertToRadians(euler[0]), XMConvertToRadians(euler[2])));

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

void Transform::SetParentWorldMatrix(const SimpleMath::Matrix& parentWorldMatrix)
{
    this->parentWorldMatrix = parentWorldMatrix;
    this->hasChanges = true;
    this->UpdateWorldMatrix();
}

void Transform::UpdateWorldMatrix()
{
    this->rotation.Normalize();
    eulerAngles = rotation.ToEuler();
    SimpleMath::Matrix scaleMatrix = SimpleMath::Matrix::CreateScale(this->scale);
    SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(this->rotation);
    SimpleMath::Matrix translationMatrix = SimpleMath::Matrix::CreateTranslation(this->position);

    // Объединяем матрицы: масштаб, потом вращение, потом перевод
    this->worldMatrix = scaleMatrix * rotationMatrix * translationMatrix * parentWorldMatrix;

    // Обновляем векторы направления
    this->vec_forward = SimpleMath::Vector3::Transform(DEFAULT_FORWARD_VECTOR, rotationMatrix * parentWorldMatrix);
    this->vec_backward = SimpleMath::Vector3::Transform(DEFAULT_BACKWARD_VECTOR, rotationMatrix * parentWorldMatrix);
    this->vec_left = SimpleMath::Vector3::Transform(DEFAULT_LEFT_VECTOR, rotationMatrix * parentWorldMatrix);
    this->vec_right = SimpleMath::Vector3::Transform(DEFAULT_RIGHT_VECTOR, rotationMatrix * parentWorldMatrix);
    this->vec_up = SimpleMath::Vector3::Transform(DEFAULT_UP_VECTOR, rotationMatrix * parentWorldMatrix);
    this->vec_down = SimpleMath::Vector3::Transform(DEFAULT_DOWN_VECTOR, rotationMatrix * parentWorldMatrix);
}

void Transform::SetLocalMatrix(SimpleMath::Matrix& matrix)
{
    this->localMatrix = matrix;
    matrix.Decompose(scale, rotation, position);
    this->hasChanges = true;
    this->UpdateWorldMatrix();
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
    rotation = quaternion;
    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::SetEulerRotate(const SimpleMath::Vector3& eulerAngle)
{
    // Convert Euler angles to quaternion with the correct order: Yaw -> Pitch -> Roll
    rotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
        XMConvertToRadians(eulerAngle.y), // Yaw (Y-axis)
        XMConvertToRadians(eulerAngle.x), // Pitch (X-axis)
        XMConvertToRadians(eulerAngle.z)  // Roll (Z-axis)
    );

    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::SetLocalRotation(const SimpleMath::Quaternion& localRotation)
{
    rotation = localRotation * rotation;
    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::AdjustRotation(const SimpleMath::Vector3& eulerAngle)
{
    SetRotation(rotation + SimpleMath::Quaternion::CreateFromYawPitchRoll(
        XMConvertToRadians(eulerAngle.y), // Yaw (Y-axis)
        XMConvertToRadians(eulerAngle.x), // Pitch (X-axis)
        XMConvertToRadians(eulerAngle.z)  // Roll (Z-axis)
    ));
}

void Transform::AdjustRotation(float roll, float pitch, float yaw)
{
    SetRotation(rotation + SimpleMath::Quaternion::CreateFromYawPitchRoll(
        XMConvertToRadians(yaw), // Yaw (Y-axis)
        XMConvertToRadians(pitch), // Pitch (X-axis)
        XMConvertToRadians(roll)  // Roll (Z-axis)
    ));
}

void Transform::SetLookAtPos(const SimpleMath::Vector3& lookAtPos)
{
    //Verify that look at pos is not the same as cam pos. They cannot be the same as that wouldn't make sense and would result in undefined behavior.
    if (lookAtPos == position)
        return;

    const auto pos = position - lookAtPos;


    float pitch = 0.0f;
    if (pos.y != 0.0f)
    {
        const float distance = sqrt(pos.x * pos.x + pos.z * pos.z);
        pitch = atan(pos.y / distance);
    }

    float yaw = 0.0f;
    if (pos.x != 0.0f)
    {
        yaw = atan(pos.x / pos.z);
    }
    if (pos.z > 0)
        yaw += XM_PI;

    this->SetRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
}

void Transform::RotateAround(const SimpleMath::Vector3& point, const SimpleMath::Quaternion& rotation)
{
    if (point == position)
        return;

    SimpleMath::Vector3 translatedPos = position - point;
    SimpleMath::Vector3 rotatedPos = SimpleMath::Vector3::Transform(translatedPos, rotation);

    position = rotatedPos + point;
    SetLookAtPos(point);

    this->hasChanges = true;
    UpdateWorldMatrix();
}

void Transform::KeepDistance(const SimpleMath::Vector3& targetPosition, float distance)
{
    SimpleMath::Vector3 direction = this->position - targetPosition;
    float currentDistance = direction.Length();

    if (currentDistance != distance)
    {
        direction.Normalize();
        SimpleMath::Vector3 scaledDirection = direction * distance;
        this->position = targetPosition + scaledDirection;
        this->hasChanges = true;
        this->UpdateWorldMatrix();
    }
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
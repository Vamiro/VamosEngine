#include "Transform.h"
#include "ColliderComponent.h"
#include "Engine/Core/GameObject.h"

Transform::Transform(GameObject& parent) : Component(parent, "Transform"), parentTransform(nullptr), hasChanges(true)
{
}

Transform::~Transform()
{
}

void Transform::Start() {}
void Transform::Update(float deltaTime)
{
    UpdateWorldMatrix();
}
void Transform::Destroy() {}

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

void Transform::SetParent(Transform* newParent)
{
    parentTransform = newParent;
    if (parentTransform)
    {
        (GetWorldMatrix() * parentTransform->GetWorldMatrix().Invert()).Decompose(scale, rotation, position);
    }
    MarkDirty();
}

void Transform::MarkDirty()
{
    hasChanges = true;
}

void Transform::UpdateWorldMatrix()
{
    if (!hasChanges && (!parentTransform || !parentTransform->hasChanges)) return;

    rotation.Normalize();
    eulerAngles = rotation.ToEuler();
    // Обновляем локальную матрицу
    const SimpleMath::Matrix scaleMatrix = SimpleMath::Matrix::CreateScale(scale);
    const SimpleMath::Matrix rotationMatrix = SimpleMath::Matrix::CreateFromQuaternion(rotation);
    const SimpleMath::Matrix translationMatrix = SimpleMath::Matrix::CreateTranslation(position);
    localMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    // Если есть родитель, мировая матрица = локальная * мировая матрица родителя
    if (parentTransform)
    {
        worldMatrix = localMatrix * parentTransform->GetWorldMatrix();
    }
    else
    {
        worldMatrix = localMatrix;
    }

    // Обновляем векторы направления
    vec_forward = SimpleMath::Vector3::Transform(DEFAULT_FORWARD_VECTOR, rotationMatrix);
    vec_right = SimpleMath::Vector3::Transform(DEFAULT_RIGHT_VECTOR, rotationMatrix);
    vec_up = SimpleMath::Vector3::Transform(DEFAULT_UP_VECTOR, rotationMatrix);

    hasChanges = false;
}

const SimpleMath::Matrix& Transform::GetWorldMatrix()
{
    return worldMatrix;
}

void Transform::SetPosition(const SimpleMath::Vector3& pos)
{
    position = pos;
    MarkDirty();
}

void Transform::SetScale(const SimpleMath::Vector3& scl)
{
    scale = scl;
    MarkDirty();
}

void Transform::SetRotation(const SimpleMath::Quaternion& quat)
{
    rotation = quat;
    MarkDirty();
}

void Transform::SetEulerRotate(const SimpleMath::Vector3& eulerAngle)
{
    rotation = SimpleMath::Quaternion::CreateFromYawPitchRoll(
        XMConvertToRadians(eulerAngle.y),
        XMConvertToRadians(eulerAngle.x),
        XMConvertToRadians(eulerAngle.z)
    );
    MarkDirty();
}

void Transform::AdjustPosition(const SimpleMath::Vector3& offset)
{
    SetPosition(position + offset);
}

void Transform::AdjustScale(const SimpleMath::Vector3& scaleFactor)
{
    SetScale(scale + scaleFactor);
}

void Transform::AdjustRotation(const SimpleMath::Vector3& eulerAngle)
{
    SetRotation(rotation * SimpleMath::Quaternion::CreateFromYawPitchRoll(
        XMConvertToRadians(eulerAngle.y),
        XMConvertToRadians(eulerAngle.x),
        XMConvertToRadians(eulerAngle.z)
    ));
}

void Transform::SetLookAtPos(const SimpleMath::Vector3& targetPos)
{
    if (targetPos == position)
        return;

    const auto pos = position - targetPos;


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

    SetRotation(SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
}

void Transform::RotateAround(const SimpleMath::Vector3& point, const SimpleMath::Quaternion& rot)
{
    if (point == position)
        return;

    SimpleMath::Vector3 translatedPos = position - point;
    SimpleMath::Vector3 rotatedPos = SimpleMath::Vector3::Transform(translatedPos, rot);

    SetPosition(rotatedPos + point);
    SetLookAtPos(point);
}

void Transform::KeepDistance(const SimpleMath::Vector3& targetPos, float desiredDistance)
{
    SimpleMath::Vector3 direction = this->position - targetPos;
    float currentDistance = direction.Length();

    if (currentDistance != desiredDistance)
    {
        direction.Normalize();
        SimpleMath::Vector3 scaledDirection = direction * desiredDistance;
        SetPosition(targetPos + scaledDirection);
    }
}

const SimpleMath::Vector3& Transform::GetForwardVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_forward;
}

const SimpleMath::Vector3& Transform::GetRightVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_right;
}

const SimpleMath::Vector3& Transform::GetUpVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_up;
}

#pragma region Local/Global Getters and Setters

// Локальные методы
const SimpleMath::Vector3& Transform::GetLocalPosition() const
{
    return position;
}

const SimpleMath::Quaternion& Transform::GetLocalRotation() const
{
    return rotation;
}

const SimpleMath::Vector3& Transform::GetLocalScale() const
{
    return scale;
}

void Transform::SetLocalPosition(const SimpleMath::Vector3& pos)
{
    SetPosition(pos);
}

void Transform::SetLocalRotation(const SimpleMath::Quaternion& rot)
{
    SetRotation(rot);
}

void Transform::SetLocalScale(const SimpleMath::Vector3& scl)
{
    SetScale(scl);
}

// Глобальные методы
SimpleMath::Vector3 Transform::GetGlobalPosition()
{
    if (hasChanges) UpdateWorldMatrix();
    return worldMatrix.Translation(); // Извлечение позиции из мировой матрицы
}

SimpleMath::Quaternion Transform::GetGlobalRotation()
{
    if (hasChanges) UpdateWorldMatrix();
    SimpleMath::Vector3 globalScale, globalPos;
    SimpleMath::Quaternion globalRotation;
    worldMatrix.Decompose(globalScale, globalRotation, globalPos);
    return globalRotation;
}

SimpleMath::Vector3 Transform::GetGlobalScale()
{
    if (hasChanges) UpdateWorldMatrix();
    SimpleMath::Vector3 globalScale, globalPos;
    SimpleMath::Quaternion globalRotation;
    worldMatrix.Decompose(globalScale, globalRotation, globalPos);
    return globalScale;
}

void Transform::SetGlobalPosition(const SimpleMath::Vector3& globalPos)
{
    if (parentTransform)
    {
        SimpleMath::Matrix invParentWorld = parentTransform->GetWorldMatrix().Invert();
        SimpleMath::Vector3 localPos = SimpleMath::Vector3::Transform(globalPos, invParentWorld);
        SetLocalPosition(localPos);
    }
    else
    {
        SetLocalPosition(globalPos);
    }
}

void Transform::SetGlobalRotation(const SimpleMath::Quaternion& globalRot)
{
    if (parentTransform)
    {
        SimpleMath::Quaternion invParentRot;
        parentTransform->GetGlobalRotation().Inverse(invParentRot);
        SimpleMath::Quaternion localRot = invParentRot * globalRot;
        SetLocalRotation(localRot);
    }
    else
    {
        SetLocalRotation(globalRot);
    }
}

void Transform::SetGlobalScale(const SimpleMath::Vector3& globalScl)
{
    if (parentTransform)
    {
        SimpleMath::Vector3 parentScale = parentTransform->GetGlobalScale();
        SetLocalScale(globalScl / parentScale); // Локальный масштаб = глобальный / родительский
    }
    else
    {
        SetLocalScale(globalScl);
    }
}

#pragma endregion

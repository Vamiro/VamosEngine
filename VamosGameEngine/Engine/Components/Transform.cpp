#include "Transform.h"
#include "ColliderComponent.h"

Transform::Transform(GameObject& parent) : Component(parent, "Transform"), parentTransform(nullptr)
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
    bool it = false;
    it |= ImGuiHelper::Vector3GUI("Position", position);
    it |= ImGuiHelper::Vector3GUI("Rotation", eulerAngles);
    it |= ImGuiHelper::Vector3GUI("Scale", scale);

    if (it) SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(eulerAngles.y),
                                                                       DirectX::XMConvertToRadians(eulerAngles.x),
                                                                       DirectX::XMConvertToRadians(eulerAngles.z)));;

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
    if (parentTransform == newParent) return;

    DirectX::SimpleMath::Vector3 globalPosition = GetGlobalPosition();
    DirectX::SimpleMath::Quaternion globalRotation = GetGlobalRotation();
    DirectX::SimpleMath::Vector3 globalScale = GetGlobalScale();

    parentTransform = newParent;

    SetGlobalPosition(globalPosition);
    SetGlobalRotation(globalRotation);
    SetGlobalScale(globalScale);
}

void Transform::MarkDirty()
{
    hasChanges = true;
}

void Transform::UpdateWorldMatrix()
{
    rotation.Normalize();
    eulerAngles = rotation.ToEuler();
    eulerAngles = DirectX::SimpleMath::Vector3(DirectX::XMConvertToDegrees(eulerAngles.x),
                                               DirectX::XMConvertToDegrees(eulerAngles.y),
                                               DirectX::XMConvertToDegrees(eulerAngles.z));

    // Обновляем локальную матрицу
    const DirectX::SimpleMath::Matrix scaleMatrix = DirectX::SimpleMath::Matrix::CreateScale(scale);
    const DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
    const DirectX::SimpleMath::Matrix translationMatrix = DirectX::SimpleMath::Matrix::CreateTranslation(position);
    localMatrix = scaleMatrix * rotationMatrix * translationMatrix;

    if (parentTransform)
    {
        worldMatrix = localMatrix * parentTransform->GetWorldMatrix();
    }
    else
    {
        worldMatrix = localMatrix;
    }

    // Обновляем векторы направления
    vec_forward = DirectX::SimpleMath::Vector3::Transform(DEFAULT_FORWARD_VECTOR, rotationMatrix);
    vec_right = DirectX::SimpleMath::Vector3::Transform(DEFAULT_RIGHT_VECTOR, rotationMatrix);
    vec_up = DirectX::SimpleMath::Vector3::Transform(DEFAULT_UP_VECTOR, rotationMatrix);

    hasChanges = false;
}

const DirectX::SimpleMath::Matrix& Transform::GetWorldMatrix()
{
    return worldMatrix;
}

void Transform::SetPosition(const DirectX::SimpleMath::Vector3& pos)
{
    position = pos;
    MarkDirty();
}

void Transform::SetScale(const DirectX::SimpleMath::Vector3& scl)
{
    scale = scl;
    MarkDirty();
}

void Transform::SetRotation(const DirectX::SimpleMath::Quaternion& quat)
{
    rotation = quat;
    MarkDirty();
}

void Transform::SetEulerRotate(const DirectX::SimpleMath::Vector3& eulerAngle)
{
    rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
        DirectX::XMConvertToRadians(eulerAngle.y),
        DirectX::XMConvertToRadians(eulerAngle.x),
        DirectX::XMConvertToRadians(eulerAngle.z)
    );
    MarkDirty();
}

void Transform::AdjustPosition(const DirectX::SimpleMath::Vector3& offset)
{
    SetPosition(position + offset);
}

void Transform::AdjustScale(const DirectX::SimpleMath::Vector3& scaleFactor)
{
    SetScale(scale + scaleFactor);
}

void Transform::AdjustRotation(const DirectX::SimpleMath::Vector3& eulerAngle)
{
    SetRotation(rotation * DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(
        DirectX::XMConvertToRadians(eulerAngle.y),
        DirectX::XMConvertToRadians(eulerAngle.x),
        DirectX::XMConvertToRadians(eulerAngle.z)
    ));
}

void Transform::SetLookAtPos(const DirectX::SimpleMath::Vector3& targetPos)
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
        yaw += DirectX::XM_PI;

    SetRotation(DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(yaw, pitch, 0.0f));
}

void Transform::RotateAround(const DirectX::SimpleMath::Vector3& point, const DirectX::SimpleMath::Vector3& axis, const float angle)
{
    DirectX::SimpleMath::Vector3 translatedPos = position - point;
    DirectX::SimpleMath::Quaternion rot = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(axis, DirectX::XMConvertToRadians(angle));
    DirectX::SimpleMath::Vector3 rotatedPos = DirectX::SimpleMath::Vector3::Transform(translatedPos, rot);

    SetPosition(rotatedPos + point);
    SetLookAtPos(point);
}

void Transform::KeepDistance(const DirectX::SimpleMath::Vector3& targetPos, float desiredDistance, float smooth)
{
    DirectX::SimpleMath::Vector3 direction = this->position - targetPos;
    float currentDistance = direction.Length();

    if (currentDistance != desiredDistance)
    {
        direction.Normalize();
        DirectX::SimpleMath::Vector3 scaledDirection = direction * desiredDistance;
        DirectX::SimpleMath::Vector3 newPosition = targetPos + scaledDirection;

        if (smooth != 0.0f)
        {
            newPosition = DirectX::SimpleMath::Vector3::Lerp(this->position, newPosition, smooth);
        }

        SetPosition(newPosition);
    }
}

const DirectX::SimpleMath::Vector3& Transform::GetForwardVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_forward;
}

const DirectX::SimpleMath::Vector3& Transform::GetRightVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_right;
}

const DirectX::SimpleMath::Vector3& Transform::GetUpVector()
{
    if (hasChanges) UpdateWorldMatrix();
    return vec_up;
}

#pragma region Local/Global Getters and Setters

// Локальные методы
const DirectX::SimpleMath::Vector3& Transform::GetLocalPosition() const
{
    return position;
}

const DirectX::SimpleMath::Quaternion& Transform::GetLocalRotation() const
{
    return rotation;
}

const DirectX::SimpleMath::Vector3& Transform::GetLocalScale() const
{
    return scale;
}

void Transform::SetLocalPosition(const DirectX::SimpleMath::Vector3& pos)
{
    SetPosition(pos);
}

void Transform::SetLocalRotation(const DirectX::SimpleMath::Quaternion& rot)
{
    SetRotation(rot);
}

void Transform::SetLocalScale(const DirectX::SimpleMath::Vector3& scl)
{
    SetScale(scl);
}

// Глобальные методы
DirectX::SimpleMath::Vector3 Transform::GetGlobalPosition()
{
    if (hasChanges) UpdateWorldMatrix();
    return worldMatrix.Translation(); // Извлечение позиции из мировой матрицы
}

DirectX::SimpleMath::Quaternion Transform::GetGlobalRotation()
{
    if (hasChanges) UpdateWorldMatrix();
    DirectX::SimpleMath::Vector3 globalScale, globalPos;
    DirectX::SimpleMath::Quaternion globalRotation;
    worldMatrix.Decompose(globalScale, globalRotation, globalPos);
    return globalRotation;
}

DirectX::SimpleMath::Vector3 Transform::GetGlobalScale()
{
    if (hasChanges) UpdateWorldMatrix();
    DirectX::SimpleMath::Vector3 globalScale, globalPos;
    DirectX::SimpleMath::Quaternion globalRotation;
    worldMatrix.Decompose(globalScale, globalRotation, globalPos);
    return globalScale;
}

void Transform::SetGlobalPosition(const DirectX::SimpleMath::Vector3& globalPos)
{
    if (parentTransform)
    {
        DirectX::SimpleMath::Matrix invParentWorld = parentTransform->GetWorldMatrix().Invert();
        DirectX::SimpleMath::Vector3 localPos = DirectX::SimpleMath::Vector3::Transform(globalPos, invParentWorld);
        SetLocalPosition(localPos);
    }
    else
    {
        SetLocalPosition(globalPos);
    }
}

void Transform::SetGlobalRotation(const DirectX::SimpleMath::Quaternion& globalRot)
{
    if (parentTransform)
    {
        DirectX::SimpleMath::Quaternion invParentRot;
        parentTransform->GetGlobalRotation().Inverse(invParentRot);
        DirectX::SimpleMath::Quaternion localRot = invParentRot * globalRot;
        SetLocalRotation(localRot);
    }
    else
    {
        SetLocalRotation(globalRot);
    }
}

void Transform::SetGlobalScale(const DirectX::SimpleMath::Vector3& globalScl)
{
    if (parentTransform)
    {
        DirectX::SimpleMath::Vector3 parentScale = parentTransform->GetGlobalScale();
        SetLocalScale(globalScl / parentScale); // Локальный масштаб = глобальный / родительский
    }
    else
    {
        SetLocalScale(globalScl);
    }
}

#pragma endregion

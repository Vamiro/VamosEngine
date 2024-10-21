#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Engine/Core/GameObject.h"

class Transform : public Component
{
public:
    Transform(GameObject& parent);
    ~Transform() override;

    void Start() override;
    void Update(float deltaTime) override;
    void Destroy() override;
    void RenderGUI() override;

    void SetParent(Transform* newParent);
    void MarkDirty();

    // Локальные трансформации
    const DirectX::SimpleMath::Vector3& GetLocalPosition() const;
    const DirectX::SimpleMath::Quaternion& GetLocalRotation() const;
    const DirectX::SimpleMath::Vector3& GetLocalScale() const;

    void SetLocalPosition(const DirectX::SimpleMath::Vector3& pos);
    void SetLocalRotation(const DirectX::SimpleMath::Quaternion& rot);
    void SetLocalScale(const DirectX::SimpleMath::Vector3& scale);

    // Глобальные трансформации
    DirectX::SimpleMath::Vector3 GetGlobalPosition();
    DirectX::SimpleMath::Quaternion GetGlobalRotation();
    DirectX::SimpleMath::Vector3 GetGlobalScale();

    void SetGlobalPosition(const DirectX::SimpleMath::Vector3& globalPos);
    void SetGlobalRotation(const DirectX::SimpleMath::Quaternion& globalRot);
    void SetGlobalScale(const DirectX::SimpleMath::Vector3& globalScale);

    void SetScale(const DirectX::SimpleMath::Vector3& scl);
    void SetRotation(const DirectX::SimpleMath::Quaternion& quat);
    void SetEulerRotate(const DirectX::SimpleMath::Vector3& eulerAngle);
    void AdjustPosition(const DirectX::SimpleMath::Vector3& offset);
    void AdjustScale(const DirectX::SimpleMath::Vector3& scaleFactor);
    void AdjustRotation(const DirectX::SimpleMath::Vector3& eulerAngle);
    // Вращение и движение
    void SetLookAtPos(const DirectX::SimpleMath::Vector3& lookAtPos);
    void RotateAround(const DirectX::SimpleMath::Vector3& point, const DirectX::SimpleMath::Vector3& axis, const float angle);
    void KeepDistance(const DirectX::SimpleMath::Vector3& targetPosition, float distance, float smooth = 0.0f);

    // Векторы направления
    const DirectX::SimpleMath::Vector3& GetForwardVector();
    const DirectX::SimpleMath::Vector3& GetRightVector();
    const DirectX::SimpleMath::Vector3& GetUpVector();

    // Обновление и получение матриц
    void UpdateWorldMatrix();
    const DirectX::SimpleMath::Matrix& GetWorldMatrix();
    void SetPosition(const DirectX::SimpleMath::Vector3& pos);

    const DirectX::SimpleMath::Vector3 DEFAULT_FORWARD_VECTOR  = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
    const DirectX::SimpleMath::Vector3 DEFAULT_RIGHT_VECTOR = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    const DirectX::SimpleMath::Vector3 DEFAULT_UP_VECTOR = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

    bool hasChanges = true;

private:
    Transform* parentTransform;
    DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix localMatrix = DirectX::SimpleMath::Matrix::Identity;

    DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Quaternion rotation = DirectX::SimpleMath::Quaternion::Identity;
    DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;
    DirectX::SimpleMath::Vector3 eulerAngles = DirectX::SimpleMath::Vector3::Zero;

    // Векторы направления
    DirectX::SimpleMath::Vector3 vec_forward;
    DirectX::SimpleMath::Vector3 vec_right;
    DirectX::SimpleMath::Vector3 vec_up;
};

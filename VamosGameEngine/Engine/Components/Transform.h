#pragma once

#include <iostream>
#include <ImGUI/imgui.h>
#include "Component.h"
#include <SimpleMath.h>

using namespace DirectX;

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
    const SimpleMath::Vector3& GetLocalPosition() const;
    const SimpleMath::Quaternion& GetLocalRotation() const;
    const SimpleMath::Vector3& GetLocalScale() const;

    void SetLocalPosition(const SimpleMath::Vector3& pos);
    void SetLocalRotation(const SimpleMath::Quaternion& rot);
    void SetLocalScale(const SimpleMath::Vector3& scale);

    // Глобальные трансформации
    SimpleMath::Vector3 GetGlobalPosition();
    SimpleMath::Quaternion GetGlobalRotation();
    SimpleMath::Vector3 GetGlobalScale();

    void SetGlobalPosition(const SimpleMath::Vector3& globalPos);
    void SetGlobalRotation(const SimpleMath::Quaternion& globalRot);
    void SetGlobalScale(const SimpleMath::Vector3& globalScale);

    void SetScale(const SimpleMath::Vector3& scl);
    void SetRotation(const SimpleMath::Quaternion& quat);
    void SetEulerRotate(const SimpleMath::Vector3& eulerAngle);
    void AdjustPosition(const SimpleMath::Vector3& offset);
    void AdjustScale(const SimpleMath::Vector3& scaleFactor);
    void AdjustRotation(const SimpleMath::Vector3& eulerAngle);
    // Вращение и движение
    void SetLookAtPos(const SimpleMath::Vector3& lookAtPos);
    void RotateAround(const SimpleMath::Vector3& point, const SimpleMath::Quaternion& rotation);
    void KeepDistance(const SimpleMath::Vector3& targetPosition, float distance);

    // Векторы направления
    const SimpleMath::Vector3& GetForwardVector();
    const SimpleMath::Vector3& GetRightVector();
    const SimpleMath::Vector3& GetUpVector();

    // Обновление и получение матриц
    void UpdateWorldMatrix();
    const SimpleMath::Matrix& GetWorldMatrix();
    void SetPosition(const SimpleMath::Vector3& pos);

    const SimpleMath::Vector3 DEFAULT_FORWARD_VECTOR  = SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
    const SimpleMath::Vector3 DEFAULT_RIGHT_VECTOR = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    const SimpleMath::Vector3 DEFAULT_UP_VECTOR = SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

    bool hasChanges = true;

private:
    Transform* parentTransform;
    SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::Identity;
    SimpleMath::Matrix localMatrix = SimpleMath::Matrix::Identity;

    SimpleMath::Vector3 position = SimpleMath::Vector3::Zero;
    SimpleMath::Quaternion rotation = SimpleMath::Quaternion::Identity;
    SimpleMath::Vector3 scale = SimpleMath::Vector3::One;
    SimpleMath::Vector3 eulerAngles = SimpleMath::Vector3::Zero;

    // Векторы направления

    SimpleMath::Vector3 vec_forward;
    SimpleMath::Vector3 vec_right;
    SimpleMath::Vector3 vec_up;
};

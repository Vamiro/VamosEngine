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
    void Update() override;
    void Destroy() override;
    void RenderGUI() override;

    const SimpleMath::Vector3& GetPositionVector() const;
    const SimpleMath::Vector3& GetScaleVector() const;
    const SimpleMath::Quaternion& GetRotationQuaternion() const;

    void SetPosition(const SimpleMath::Vector3& pos);
    void AdjustPosition(const SimpleMath::Vector3& pos);

    void SetScale(const SimpleMath::Vector3& scale);
    void AdjustScale(const SimpleMath::Vector3& scale);

    void SetRotation(SimpleMath::Quaternion quaternion);
    void SetEulerRotate(const SimpleMath::Vector3& eulerAngle);
    void SetLocalRotation(const SimpleMath::Quaternion& localRotation);
    void AdjustRotation(const SimpleMath::Vector3& eulerAngle);
    void AdjustRotation(float roll, float pitch, float yaw);

    void SetLookAtPos(const SimpleMath::Vector3& lookAtPos);
    void RotateAround(const SimpleMath::Vector3& point, const SimpleMath::Quaternion& rotation);
    void KeepDistance(const SimpleMath::Vector3& targetPosition, float distance);

    const SimpleMath::Vector3& GetForwardVector();
    const SimpleMath::Vector3& GetRightVector();
    const SimpleMath::Vector3& GetBackwardVector();
    const SimpleMath::Vector3& GetLeftVector();
    const SimpleMath::Vector3& GetUpVector();
    const SimpleMath::Vector3& GetDownVector();

    void SetParentWorldMatrix(const SimpleMath::Matrix& parentWorldMatrix);
    void UpdateWorldMatrix();
    void SetLocalMatrix(SimpleMath::Matrix& matrix);
    [[nodiscard]] SimpleMath::Matrix GetLocalMatrix() const { return localMatrix; }
    [[nodiscard]] SimpleMath::Matrix GetWorldMatrix() const { return worldMatrix; }


    const SimpleMath::Vector3 DEFAULT_FORWARD_VECTOR = SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
    const SimpleMath::Vector3 DEFAULT_BACKWARD_VECTOR = SimpleMath::Vector3(0.0f, 0.0f, -1.0f);
    const SimpleMath::Vector3 DEFAULT_LEFT_VECTOR = SimpleMath::Vector3(-1.0f, 0.0f, 0.0f);
    const SimpleMath::Vector3 DEFAULT_RIGHT_VECTOR = SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
    const SimpleMath::Vector3 DEFAULT_UP_VECTOR = SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
    const SimpleMath::Vector3 DEFAULT_DOWN_VECTOR = SimpleMath::Vector3(0.0f, -1.0f, 0.0f);

private:
    SimpleMath::Matrix worldMatrix = SimpleMath::Matrix::Identity;
    SimpleMath::Matrix localMatrix = SimpleMath::Matrix::Identity;
    SimpleMath::Matrix parentWorldMatrix = SimpleMath::Matrix::Identity;

    SimpleMath::Vector3 scale = SimpleMath::Vector3::One;
    SimpleMath::Vector3 position = SimpleMath::Vector3::Zero;
    SimpleMath::Quaternion rotation = SimpleMath::Quaternion::Identity;
    SimpleMath::Vector3 eulerAngles = SimpleMath::Vector3::Zero;

    SimpleMath::Vector3 vec_forward;
    SimpleMath::Vector3 vec_left;
    SimpleMath::Vector3 vec_right;
    SimpleMath::Vector3 vec_backward;
    SimpleMath::Vector3 vec_up;
    SimpleMath::Vector3 vec_down;

    bool hasChanges;
};
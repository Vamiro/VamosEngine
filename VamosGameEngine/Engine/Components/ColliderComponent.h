#pragma once
#include "Transform.h"
#include "Engine/Utilities/Delegate/Delegate.h"
#include "Engine/Physics/PhysicsEngine.h"

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject& parent,
                        JPH::Shape* shape,
                        DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f),
                        JPH::EMotionType motionType = JPH::EMotionType::Static,
                        JPH::ObjectLayer layer = Layers::NON_MOVING,
                        bool allowSleeping = true,
                        bool isTrigger = false);

    ~ColliderComponent();

    void Start() override;
    void Update(float deltaTime) override;
    void UpdatePhysics(float deltaTime);
    void MoveKinematic(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation,
                       float deltaTime) const;
    DirectX::SimpleMath::Matrix GetColliderTransform();
    void RenderGUI() override;
    void SetActivation(bool active);

    [[nodiscard]] DirectX::SimpleMath::Vector3 GetScale() const;
    JPH::Vec3 CalcScale() const;
    void SetScale(const DirectX::SimpleMath::Vector3& scale);
    void SetScale(float radius);
    void UpdateScale();
    [[nodiscard]] JPH::BodyID GetID() const;
    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPosition() const;
    [[nodiscard]] DirectX::SimpleMath::Quaternion GetRotation() const;
    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPositionOffset() const;
    [[nodiscard]] DirectX::SimpleMath::Quaternion GetRotationOffset() const;
    void SetPositionAndRotation(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) const;
    void SetPosition(const DirectX::SimpleMath::Vector3& position) const;
    void SetRotation(const DirectX::SimpleMath::Quaternion& rotation) const;
    void SetLayer(uint16_t layer);
    void SetMotionType(JPH::EMotionType motionType);

    Delegate<void, ColliderComponent*>& OnCollision() { return onCollision_; }
    void Destroy() override;
    void SetTrigger(bool isTrigger);
    bool IsActive() const { return isActive; }

private:
    bool DestroyFlag = false;
    Delegate<void, ColliderComponent*> onCollision_;

    JPH::BodyID mBodyID;
    JPH::Shape* mShape;
    JPH::BodyCreationSettings mSettings;

    JPH::Vec3 mPositionOffset = JPH::Vec3::sZero();
    JPH::Quat mRotationOffset = JPH::Quat::sIdentity();
    JPH::Vec3 mScale = JPH::Vec3::sReplicate(1.0f);
    JPH::EMotionType mMotionType;
    JPH::ObjectLayer mLayer;
    bool mAllowSleeping;
    bool mIsTrigger;
    bool isActive;
};

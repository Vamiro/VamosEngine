#pragma once

#include "Component.h"
#include "Delegate.h"
#include "SimpleMath.h"
#include "Engine/Physics/PhysicsEngine.h"

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject& parent, JPH::PhysicsSystem* physicsSystem,
    JPH::EMotionType motionType, JPH::ObjectLayer layer, bool allowSleeping = true,
                                 bool isTrigger = false);
    ~ColliderComponent();

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void SetActivation(bool active) const;

    void SetShape(JPH::Shape* shape);
    void SetShape(const std::string& newShape);
    [[nodiscard]] DirectX::SimpleMath::Vector3 GetScale() const;
    void SetScale(const DirectX::SimpleMath::Vector3& scale);
    void SetScale(float radius);
    [[nodiscard]] JPH::Body* GetBody() const;
    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPosition() const;
    [[nodiscard]] DirectX::SimpleMath::Quaternion GetRotation() const;
    [[nodiscard]] DirectX::SimpleMath::Vector3 GetPositionOffset() const;
    [[nodiscard]] DirectX::SimpleMath::Quaternion GetRotationOffset() const;
    void SetPositionAndRotation(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) const;
    void SetPosition(const DirectX::SimpleMath::Vector3& position) const;
    void SetRotation(const DirectX::SimpleMath::Quaternion& rotation) const;

    Delegate<void, ColliderComponent*>& OnCollision() { return onCollision_; }
    void Destroy() override;

private:

    Delegate<void, ColliderComponent*> onCollision_;

    JPH::Body* mBody;
    JPH::Shape* mShape;
    JPH::BodyCreationSettings mSettings;
    JPH::PhysicsSystem* mPhysicsSystem;

    JPH::Vec3 mPositionOffset = JPH::Vec3::sZero();
    JPH::Quat mRotationOffset = JPH::Quat::sIdentity();
    JPH::Vec3 mScale = JPH::Vec3::sReplicate(1.0f);
    JPH::EMotionType mMotionType;
    JPH::ObjectLayer mLayer;
    bool mAllowSleeping;
    bool mIsTrigger;
};

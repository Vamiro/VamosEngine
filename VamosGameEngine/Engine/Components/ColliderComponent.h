#pragma once

#include "Component.h"
#include <Engine/Physics/PhysicsEngine.h>
#include "Engine/Core/Object.h"

class ColliderComponent : public Component {
public:
    ColliderComponent(Object& parent, JPH::PhysicsSystem* physicsSystem,
        JPH::EMotionType motionType, JPH::ObjectLayer layer);
    ~ColliderComponent();

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void SetActivation(bool active) const;

    void SetShape(JPH::Shape* shape);
    void SetShape(const std::string& newShape);
    [[nodiscard]] SimpleMath::Vector3 GetScale() const;
    void SetScale(const SimpleMath::Vector3& scale);
    void SetScale(float radius);
    [[nodiscard]] JPH::Body* GetBody() const;
    [[nodiscard]] SimpleMath::Vector3 GetPosition() const;
    [[nodiscard]] SimpleMath::Quaternion GetRotation() const;
    [[nodiscard]] SimpleMath::Vector3 GetPositionOffset() const;
    [[nodiscard]] SimpleMath::Quaternion GetRotationOffset() const;
    void SetPositionAndRotation(const SimpleMath::Vector3& position, const SimpleMath::Quaternion& rotation) const;

private:
    JPH::Body* mBody;
    JPH::Shape* mShape;
    JPH::BodyCreationSettings mSettings;
    JPH::PhysicsSystem* mPhysicsSystem;

    JPH::Vec3 mPositionOffset = JPH::Vec3::sZero();
    JPH::Quat mRotationOffset = JPH::Quat::sIdentity();
    JPH::Vec3 mScale = JPH::Vec3::sReplicate(1.0f);
    JPH::EMotionType mMotionType;
    JPH::ObjectLayer mLayer;
};

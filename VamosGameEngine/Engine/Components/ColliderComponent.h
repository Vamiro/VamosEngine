#pragma once

#include "Component.h"
#include <Engine/Physics/PhysicsEngine.h>
#include "Engine/Core/Object.h"

class ColliderComponent : public Component {
public:
    ColliderComponent(Object& parent, const char* name, JPH::PhysicsSystem* physicsSystem,
        JPH::EMotionType motionType, JPH::ObjectLayer layer);
    ~ColliderComponent();

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void SetActivation(bool active) const;

    void SetShape(JPH::Shape* shape);
    void SetScale(const SimpleMath::Vector3& scale);
    JPH::Body* GetBody() const;
    void SetPositionAndRotation(const SimpleMath::Vector3& position, const SimpleMath::Quaternion& rotation);

private:
    JPH::Body* mBody;
    JPH::Shape* mShape;
    JPH::BodyCreationSettings mSettings;
    JPH::PhysicsSystem* mPhysicsSystem;

    JPH::Vec3 mPosition;
    JPH::Vec3 mRotation;
    JPH::Vec3 mScale;
    JPH::EMotionType mMotionType;
    JPH::ObjectLayer mLayer;
};

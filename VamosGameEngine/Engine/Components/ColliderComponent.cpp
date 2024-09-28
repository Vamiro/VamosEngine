#include "ColliderComponent.h"

#include "Engine/Core/GameEngine.h"


ColliderComponent::ColliderComponent(Object& parent, const char* name, JPH::PhysicsSystem* physicsSystem,
                                     JPH::EMotionType motionType,  JPH::ObjectLayer layer):
        Component(parent, name),
        mBody(nullptr),
        mShape(nullptr),
        mPhysicsSystem(physicsSystem),
        mMotionType(motionType),
        mLayer(layer)
{}

ColliderComponent::~ColliderComponent() {
    delete mShape;
}

void ColliderComponent::Start() {
    if (mShape) {
        mSettings = JPH::BodyCreationSettings(mShape, JPH::RVec3::sZero(), JPH::Quat::sIdentity(), mMotionType, mLayer);
        JPH::BodyInterface& bodyInterface = mPhysicsSystem->GetBodyInterface();
        mBody = bodyInterface.CreateBody(mSettings);
        bodyInterface.AddBody(mBody->GetID(), JPH::EActivation::DontActivate);
        const auto& quat = parent->transform->GetRotationQuaternion();
        const auto& pos = parent->transform->GetPositionVector();
        mPhysicsSystem->GetBodyInterface().SetPositionAndRotation(mBody->GetID(),
                                                                  JPH::Vec3(pos.x, pos.y, pos.z),
                                                                  JPH::Quat(quat.x, quat.y, quat.z, quat.w),
                                                                  JPH::EActivation::Activate);
    }
}

void ColliderComponent::Update() {
    // Update logic for the collider component if necessary
    if (mBody->IsActive()) {
        JPH::Vec3 pos;
        JPH::Quat quat;
        mPhysicsSystem->GetBodyInterface().GetPositionAndRotation(mBody->GetID(), pos, quat);
        parent->transform->SetPosition(SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetRotation(SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    }
}

void ColliderComponent::RenderGUI() {
    // Render GUI for the collider component if necessary
}

void ColliderComponent::SetActivation(const bool active) const
{
    if(active){
        mPhysicsSystem->GetBodyInterface().ActivateBody(mBody->GetID());
    } else {
        mPhysicsSystem->GetBodyInterface().DeactivateBody(mBody->GetID());
    }
}

void ColliderComponent::SetShape(JPH::Shape* newShape) {
    mShape = newShape;
}

void ColliderComponent::SetScale(const SimpleMath::Vector3& scale) {
    mScale = JPH::Vec3(scale.x, scale.y, scale.z);
    mShape->ScaleShape(mScale);
}

JPH::Body* ColliderComponent::GetBody() const {
    return mBody;
}

void ColliderComponent::SetPositionAndRotation(const SimpleMath::Vector3& position, const SimpleMath::Quaternion& rotation) {
    mPhysicsSystem->GetBodyInterface().SetPositionAndRotation(mBody->GetID(),
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::DontActivate);
}
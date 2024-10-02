#include "ColliderComponent.h"

#include "Transform.h"
#include "Engine/Core/GameObject.h"

ColliderComponent::ColliderComponent(GameObject& parent,
    JPH::BodyInterface& body_interface,
    JPH::EMotionType motionType,
    uint16_t layer = Layers::NON_MOVING,
    bool allowSleeping,
    bool isTrigger):
        Component(parent, "ColliderComponent"),
        mShape(nullptr),
        mBodyInterface(body_interface),
        mMotionType(motionType),
        mLayer(layer),
        mAllowSleeping(allowSleeping),
        mIsTrigger(isTrigger)
{}

ColliderComponent::~ColliderComponent() {
    delete mShape;
}

void ColliderComponent::Start() {
    if (mShape) {
        mSettings = JPH::BodyCreationSettings(mShape->ScaleShape(mScale).Get(), JPH::RVec3::sZero(), JPH::Quat::sIdentity(), mMotionType, mLayer);
        mSettings.mAllowSleeping = mAllowSleeping;
        mSettings.mIsSensor = mIsTrigger;
        mSettings.mFriction = 10.0f;

        mBodyID = mBodyInterface.CreateAndAddBody(mSettings, JPH::EActivation::DontActivate);
        const auto& quat = parent->transform->GetRotationQuaternion();
        const auto& pos = parent->transform->GetPositionVector();

        mBodyInterface.SetPositionAndRotation(
            mBodyID,
            JPH::Vec3(pos.x, pos.y, pos.z),
            JPH::Quat(quat.x, quat.y, quat.z, quat.w),
            JPH::EActivation::DontActivate);

        mBodyInterface.SetUserData(mBodyID, reinterpret_cast<JPH::uint64>(this));
        OnCollision().BindLambda([](const ColliderComponent* other){}); // Empty lambda
    }
}

void ColliderComponent::Update()
{
    // Update logic for the collider component if necessary
    if(DestroyFlag)
    {
        Destroy();
    }

    if (mMotionType == JPH::EMotionType::Dynamic) {
        JPH::Vec3 pos{};
        JPH::Quat quat{};
        mBodyInterface.GetPositionAndRotation(mBodyID, pos, quat);
        parent->transform->SetPosition(SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetRotation(SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    }
}

void ColliderComponent::RenderGUI() {
    float scale[3] = {this->mScale.GetX(), this->mScale.GetY(), this->mScale.GetZ()};

    if(mShape->GetSubType() == JPH::EShapeSubType::Sphere){
        ImGui::DragFloat("Radius", scale, 0.1f, 0.1f, 10000.0f);
        if (ImGui::IsMouseDragging(0) && ImGui::IsItemActive()) {
            SetScale(scale[0]);
        }

    } else
    {
        ImGui::DragFloat3("Scale", scale, 0.1f, 0.1f, 10000.0f);
        if (ImGui::IsMouseDragging(0) && ImGui::IsItemActive()) {
            SetScale(SimpleMath::Vector3(scale[0], scale[1], scale[2]));
        }
    }
    ImGui::Text(mBodyInterface.IsActive(mBodyID) ? "Active" : "Inactive");
    auto pos = mBodyInterface.GetPosition(mBodyID);
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.GetX(), pos.GetY(), pos.GetZ());
}

void ColliderComponent::SetActivation(const bool active) const
{
    if(active){
        mBodyInterface.ActivateBody(mBodyID);
    } else {
        mBodyInterface.DeactivateBody(mBodyID);
    }
}

void ColliderComponent::SetShape(JPH::Shape* newShape) {
    mShape = newShape;
}

void ColliderComponent::SetShape(const std::string& newShape) {
    if (newShape == "Sphere")
    {
        mShape = new JPH::SphereShape(1.0f);
    }
    else if (newShape == "Box")
    {
        mShape = new JPH::BoxShape(JPH::Vec3(1.0f, 1.0f, 1.0f));
    }
}

SimpleMath::Vector3 ColliderComponent::GetScale() const {
    return {mScale.GetX(), mScale.GetY(), mScale.GetZ()};
}

void ColliderComponent::SetScale(const SimpleMath::Vector3& scale) {
    mScale = JPH::Vec3(scale.x, scale.y, scale.z);
    auto newShape = mShape->ScaleShape(mScale);
    mBodyInterface.SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::DontActivate);
}

void ColliderComponent::SetScale(const float radius) {
    mScale = JPH::Vec3(radius, radius, radius);
    auto newShape = mShape->ScaleShape(mScale);
    mBodyInterface.SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::DontActivate);
}

JPH::BodyID ColliderComponent::GetID() const {
    return mBodyID;
}

SimpleMath::Vector3 ColliderComponent::GetPosition() const {
    const JPH::Vec3 pos = mBodyInterface.GetPosition(mBodyID);
    return {pos.GetX(), pos.GetY(), pos.GetZ()};
}

SimpleMath::Quaternion ColliderComponent::GetRotation() const {
    const JPH::Quat quat = mBodyInterface.GetRotation(mBodyID);
    return {quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()};
}

SimpleMath::Vector3 ColliderComponent::GetPositionOffset() const {
    return {mPositionOffset.GetX(), mPositionOffset.GetY(), mPositionOffset.GetZ()};
}

SimpleMath::Quaternion ColliderComponent::GetRotationOffset() const {
    return {mRotationOffset.GetX(), mRotationOffset.GetY(), mRotationOffset.GetZ(), mRotationOffset.GetW()};
}

void ColliderComponent::SetPositionAndRotation(const SimpleMath::Vector3& position, const SimpleMath::Quaternion& rotation) const
{
    mBodyInterface.SetPositionAndRotation(mBodyID,
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::DontActivate);

}

void ColliderComponent::SetPosition(const SimpleMath::Vector3& position) const
{
    mBodyInterface.SetPosition(mBodyID,
        JPH::Vec3(position.x, position.y, position.z), JPH::EActivation::DontActivate);

}

void ColliderComponent::SetRotation(const SimpleMath::Quaternion& rotation) const
{
    mBodyInterface.SetRotation(mBodyID,
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::DontActivate);
}

void ColliderComponent::Destroy()
{
    if(DestroyFlag)
    {
        mBodyInterface.RemoveBody(mBodyID);
        DestroyFlag = false;
    } else {
        DestroyFlag = true;
    }
}
#include "ColliderComponent.h"

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
        mSettings.mAllowDynamicOrKinematic = true;

        mBodyID = mBodyInterface.CreateAndAddBody(mSettings, JPH::EActivation::Activate);
        const auto& quat = parent->transform->GetGlobalRotation();
        const auto& pos = parent->transform->GetGlobalPosition();

        mBodyInterface.SetPositionAndRotation(
            mBodyID,
            JPH::Vec3(pos.x, pos.y, pos.z),
            JPH::Quat(quat.x, quat.y, quat.z, quat.w),
            JPH::EActivation::Activate);

        mBodyInterface.SetUserData(mBodyID, reinterpret_cast<JPH::uint64>(this));
        OnCollision().BindLambda([](const ColliderComponent* other){}); // Empty lambda
    }
}

void ColliderComponent::Update(float deltaTime)
{
    // Update logic for the collider component if necessary
    if(DestroyFlag)
    {
        Destroy();
        return;
    }

    if (mMotionType == JPH::EMotionType::Dynamic) {
        JPH::Vec3 pos{};
        JPH::Quat quat{};
        mBodyInterface.GetPositionAndRotation(mBodyID, pos, quat);
        parent->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetGlobalRotation(DirectX::SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    } else if (mMotionType == JPH::EMotionType::Kinematic && mLayer == Layers::PLAYER) {
        MoveKinematic(parent->transform->GetGlobalPosition(), parent->transform->GetGlobalRotation(), deltaTime);
        JPH::Vec3 pos{};
        JPH::Quat quat{};
        mBodyInterface.GetPositionAndRotation(mBodyID, pos, quat);
        parent->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetGlobalRotation(DirectX::SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    }
}

void ColliderComponent::MoveKinematic(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation, float deltaTime) const
{
    mBodyInterface.MoveKinematic(
        mBodyID,
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
        deltaTime);
}


void ColliderComponent::RenderGUI() {
    switch (mLayer)
    {
        case Layers::PLAYER:
            ImGui::Text("Layer: Player");
            break;
        case Layers::NON_MOVING:
            ImGui::Text("Layer: Non-Moving");
            break;
        case Layers::MOVING:
            ImGui::Text("Layer: Moving");
            break;
        default: break;
    }
    switch (mMotionType)
    {
        case JPH::EMotionType::Dynamic:
            ImGui::Text("Motion Type: Dynamic");
            break;
        case JPH::EMotionType::Static:
            ImGui::Text("Motion Type: Static");
            break;
        case JPH::EMotionType::Kinematic:
            ImGui::Text("Motion Type: Kinematic");
            break;
    }
    ImGui::Text(mBodyInterface.IsActive(mBodyID) ? "Active" : "Inactive");
    auto pos = mBodyInterface.GetPosition(mBodyID);
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.GetX(), pos.GetY(), pos.GetZ());

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
            SetScale(DirectX::SimpleMath::Vector3(scale[0], scale[1], scale[2]));
        }
    }
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

DirectX::SimpleMath::Vector3 ColliderComponent::GetScale() const {
    return {mScale.GetX(), mScale.GetY(), mScale.GetZ()};
}

void ColliderComponent::SetScale(const DirectX::SimpleMath::Vector3& scale) {
    mScale = JPH::Vec3(scale.x, scale.y, scale.z);
    auto newShape = mShape->ScaleShape(mScale);
    mBodyInterface.SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::Activate);
}

void ColliderComponent::SetScale(const float radius) {
    mScale = JPH::Vec3(radius, radius, radius);
    auto newShape = mShape->ScaleShape(mScale);
    mBodyInterface.SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::Activate);
}

JPH::BodyID ColliderComponent::GetID() const {
    return mBodyID;
}

DirectX::SimpleMath::Vector3 ColliderComponent::GetPosition() const {
    const JPH::Vec3 pos = mBodyInterface.GetPosition(mBodyID);
    return {pos.GetX(), pos.GetY(), pos.GetZ()};
}

DirectX::SimpleMath::Quaternion ColliderComponent::GetRotation() const {
    const JPH::Quat quat = mBodyInterface.GetRotation(mBodyID);
    return {quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()};
}

DirectX::SimpleMath::Vector3 ColliderComponent::GetPositionOffset() const {
    return {mPositionOffset.GetX(), mPositionOffset.GetY(), mPositionOffset.GetZ()};
}

DirectX::SimpleMath::Quaternion ColliderComponent::GetRotationOffset() const {
    return {mRotationOffset.GetX(), mRotationOffset.GetY(), mRotationOffset.GetZ(), mRotationOffset.GetW()};
}

void ColliderComponent::SetPositionAndRotation(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) const
{
    mBodyInterface.SetPositionAndRotation(mBodyID,
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
        JPH::EActivation::Activate);
}

void ColliderComponent::SetPosition(const DirectX::SimpleMath::Vector3& position) const
{
    mBodyInterface.SetPosition(mBodyID,
        JPH::Vec3(position.x, position.y, position.z), JPH::EActivation::Activate);

}

void ColliderComponent::SetRotation(const DirectX::SimpleMath::Quaternion& rotation) const
{
    mBodyInterface.SetRotation(mBodyID,
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::Activate);
}

void ColliderComponent::SetLayer(const uint16_t layer)
{
    mLayer = layer;
    mBodyInterface.SetObjectLayer(mBodyID, layer);
}

void ColliderComponent::SetMotionType(const JPH::EMotionType motionType)
{
    mMotionType = motionType;
    mBodyInterface.SetMotionType(mBodyID, motionType, JPH::EActivation::Activate);
}

void ColliderComponent::Destroy()
{
    if(DestroyFlag)
    {
        mShape->Release();
        mBodyInterface.RemoveBody(mBodyID);
        mBodyInterface.DestroyBody(mBodyID);
        DestroyFlag = false;
    } else {
        DestroyFlag = true;
    }
}
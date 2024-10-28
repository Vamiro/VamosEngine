#include "ColliderComponent.h"
#include "Engine/Core/Engine.h"

ColliderComponent::ColliderComponent(GameObject& parent, JPH::Shape* shape, DirectX::SimpleMath::Vector3 scale,
    JPH::EMotionType motionType, JPH::ObjectLayer layer, bool allowSleeping, bool isTrigger) :
        Component(parent, "ColliderComponent"),
        mShape(shape),
        mScale({scale.x, scale.y, scale.z}),
        mMotionType(motionType),
        mLayer(layer),
        mAllowSleeping(allowSleeping),
        mIsTrigger(isTrigger)
{
    OnCollision().BindLambda([&parent](ColliderComponent* other){ parent.OnCollisionEnter(other); });
    if (mMotionType == JPH::EMotionType::Static) return;
    parent.RegisterDynamicCollider(this);
}

ColliderComponent::~ColliderComponent() {
    delete mShape;
}

void ColliderComponent::Start() {
    if (mShape) {
        auto pos = parent->transform->GetGlobalPosition();
        auto rot = parent->transform->GetGlobalRotation();
        mSettings = JPH::BodyCreationSettings(mShape->ScaleShape(CalcScale()).Get(), {pos.x, pos.y, pos.z}, {rot.x, rot.y, rot.z, rot.w}, mMotionType, mLayer);
        mSettings.mAllowSleeping = mAllowSleeping;
        mSettings.mIsSensor = mIsTrigger;
        mSettings.mFriction = 10.0f;
        mSettings.mAllowDynamicOrKinematic = true;

        mBodyID = Engine::GetBodyInterface().CreateAndAddBody(mSettings, JPH::EActivation::Activate);
        Engine::GetBodyInterface().SetUserData(mBodyID, reinterpret_cast<JPH::uint64>(this));
    }
}

void ColliderComponent::Update(float deltaTime)
{
    if(DestroyFlag)
    {
        Destroy();
        return;
    }

    SetPositionAndRotation(parent->transform->GetGlobalPosition(), parent->transform->GetGlobalRotation());
    UpdateScale();
}

void ColliderComponent::UpdatePhysics(float deltaTime)
{
    if (mMotionType == JPH::EMotionType::Dynamic) {
        JPH::Vec3 pos{};
        JPH::Quat quat{};
        Engine::GetBodyInterface().GetPositionAndRotation(mBodyID, pos, quat);
        parent->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetGlobalRotation(DirectX::SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    } else if (mMotionType == JPH::EMotionType::Kinematic && mLayer == Layers::PLAYER) {
        MoveKinematic(parent->transform->GetGlobalPosition(), parent->transform->GetGlobalRotation(), deltaTime);
        JPH::Vec3 pos{};
        JPH::Quat quat{};
        Engine::GetBodyInterface().GetPositionAndRotation(mBodyID, pos, quat);
        parent->transform->SetGlobalPosition(DirectX::SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
        parent->transform->SetGlobalRotation(DirectX::SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()));
    }
}

void ColliderComponent::MoveKinematic(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation, float deltaTime) const
{
    Engine::GetBodyInterface().MoveKinematic(
        mBodyID,
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
        deltaTime);
}

SimpleMath::Matrix ColliderComponent::GetColliderTransform()
{
    JPH::Vec3 pos{};
    JPH::Quat quat{};
    Engine::GetBodyInterface().GetPositionAndRotation(mBodyID, pos, quat);
    return SimpleMath::Matrix::CreateFromQuaternion(SimpleMath::Quaternion(quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW())) *
        SimpleMath::Matrix::CreateTranslation(SimpleMath::Vector3(pos.GetX(), pos.GetY(), pos.GetZ()));
}

void ColliderComponent::SetActivation(const bool active)
{
    isActive = active;
    if(active){
        Engine::GetBodyInterface().ActivateBody(mBodyID);
    } else {
        Engine::GetBodyInterface().DeactivateBody(mBodyID);
    }
}

DirectX::SimpleMath::Vector3 ColliderComponent::GetScale() const {
    return {mScale.GetX(), mScale.GetY(), mScale.GetZ()};
}

JPH::Vec3 ColliderComponent::CalcScale() const {
    auto scale = parent->transform->GetGlobalScale();

    scale.x = scale.x == 0 ? 0.01f : scale.x;
    scale.y = scale.y == 0 ? 0.01f : scale.y;
    scale.z = scale.z == 0 ? 0.01f : scale.z;
    return {mScale.GetX() * abs(scale.x) , mScale.GetY() * abs(scale.y), mScale.GetZ() * abs(scale.z)};
}

void ColliderComponent::SetScale(const DirectX::SimpleMath::Vector3& scale) {
    mScale = JPH::Vec3(scale.x, scale.y, scale.z);
    auto newShape = mShape->ScaleShape(CalcScale());
    Engine::GetBodyInterface().SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::Activate);
}

void ColliderComponent::SetScale(const float radius) {
    mScale = JPH::Vec3(radius, radius, radius);
    auto newShape = mShape->ScaleShape(CalcScale());
    Engine::GetBodyInterface().SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::Activate);
}

void ColliderComponent::UpdateScale() {
    auto newShape = mShape->ScaleShape(CalcScale());
    Engine::GetBodyInterface().SetShape(mBodyID, newShape.Get(), true , JPH::EActivation::Activate);
}

JPH::BodyID ColliderComponent::GetID() const {
    return mBodyID;
}

DirectX::SimpleMath::Vector3 ColliderComponent::GetPosition() const {
    const JPH::Vec3 pos = Engine::GetBodyInterface().GetPosition(mBodyID);
    return {pos.GetX(), pos.GetY(), pos.GetZ()};
}

DirectX::SimpleMath::Quaternion ColliderComponent::GetRotation() const {
    const JPH::Quat quat = Engine::GetBodyInterface().GetRotation(mBodyID);
    return {quat.GetX(), quat.GetY(), quat.GetZ(), quat.GetW()};
}

void ColliderComponent::SetPositionAndRotation(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Quaternion& rotation) const
{
    Engine::GetBodyInterface().SetPositionAndRotation(mBodyID,
        JPH::Vec3(position.x, position.y, position.z),
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w),
        JPH::EActivation::Activate);
}

void ColliderComponent::SetPosition(const DirectX::SimpleMath::Vector3& position) const
{
    Engine::GetBodyInterface().SetPosition(mBodyID,
        JPH::Vec3(position.x, position.y, position.z), JPH::EActivation::Activate);

}

void ColliderComponent::SetRotation(const DirectX::SimpleMath::Quaternion& rotation) const
{
    Engine::GetBodyInterface().SetRotation(mBodyID,
        JPH::Quat(rotation.x, rotation.y, rotation.z, rotation.w), JPH::EActivation::Activate);
}

void ColliderComponent::SetLayer(const uint16_t layer)
{
    mLayer = layer;
    Engine::GetBodyInterface().SetObjectLayer(mBodyID, layer);
}

void ColliderComponent::SetMotionType(const JPH::EMotionType motionType)
{
    mMotionType = motionType;
    Engine::GetBodyInterface().SetMotionType(mBodyID, motionType, JPH::EActivation::Activate);
}

void ColliderComponent::Destroy()
{
    if(DestroyFlag)
    {
        mShape->Release();
        Engine::GetBodyInterface().RemoveBody(mBodyID);
        Engine::GetBodyInterface().DestroyBody(mBodyID);
        DestroyFlag = false;
    } else {
        DestroyFlag = true;
    }
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
    default: throw std::out_of_range("Invalid layer");
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
    default: throw std::out_of_range("Invalid motion type");
    }

    ImGui::Text(Engine::GetBodyInterface().IsActive(mBodyID) ? "Active" : "Inactive");
    auto pos = Engine::GetBodyInterface().GetPosition(mBodyID);
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.GetX(), pos.GetY(), pos.GetZ());

    float scale[3] = {mScale.GetX(), mScale.GetY(), mScale.GetZ()};
    JPH::Vec3 scaleVec = CalcScale();

    if(mShape->GetSubType() == JPH::EShapeSubType::Sphere){
        ImGui::DragFloat("Radius", scale, 0.1f, 0.1f, 10000.0f);
        if (ImGui::IsMouseDragging(0) && ImGui::IsItemActive()) {
            SetScale(scale[0]);
        }

        Engine::GetDebugRenderSys().DrawSphere(scale[0],
                                               SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f),
                                               GetColliderTransform(),
                                               30);
    } else
    {
        ImGui::DragFloat3("Scale", scale, 0.1f, 0.1f, 10000.0f);
        if (ImGui::IsMouseDragging(0) && ImGui::IsItemActive()) {
            SetScale(DirectX::SimpleMath::Vector3(scale[0], scale[1], scale[2]));
        }

        Engine::GetDebugRenderSys().DrawBoundingBox(BoundingBox(),SimpleMath::Matrix::CreateScale(scaleVec.GetX(), scaleVec.GetY(), scaleVec.GetZ()) * GetColliderTransform(), SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
    }
}

#include "SphereCollider.h"

#include "Engine/Core/Object.h"

SphereCollider::SphereCollider(Object& parent, const float radius,
    JPH::PhysicsSystem& physicsSystem, const DirectX::SimpleMath::Vector3& initPosition) :
        Component(parent, "SphereCollider"),
        mPosition(initPosition),
        sphereShape(nullptr),
        radius(radius),
        physicsSystem(&physicsSystem)
{
}

SphereCollider::~SphereCollider()
{
    // Удаляем сферическую форму и физическое тело при уничтожении компонента
    if (physicsSystem && !bodyID.IsInvalid())
    {
        physicsSystem->GetBodyInterface().DestroyBody(bodyID);
    }
    delete sphereShape;
}

void SphereCollider::Start()
{
    auto pos = parent->transform->GetPositionVector();

    // Создаем сферическую форму
    sphereShape = new JPH::SphereShape(radius);

    // Настройки для создания тела с коллайдером
    JPH::BodyCreationSettings bodySettings(sphereShape, {mPosition.x + pos.x, mPosition.y + pos.y, mPosition.z + pos.z}, JPH::Quat::sIdentity(), JPH::EMotionType::Static, PhysicsLayers::MOVING);

    // Создаем физическое тело и добавляем его в систему
    bodyID = physicsSystem->GetBodyInterface().CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

void SphereCollider::Update()
{
    UpdatePosition();
}

void SphereCollider::UpdatePosition() const
{
    // Обновляем позицию физического тела
    if (physicsSystem && !bodyID.IsInvalid())
    {
        auto pos = parent->transform->GetPositionVector();
        physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, {pos.x, pos.y, pos.z}, JPH::Quat::sIdentity(), JPH::EActivation::Activate);
    }
}

void SphereCollider::SetPosition(const SimpleMath::Vector3& position)
{
    mPosition = position;
    auto pos = parent->transform->GetPositionVector();

    // Обновляем позицию физического тела
    if (physicsSystem && !bodyID.IsInvalid())
    {
        physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, {mPosition.x + pos.x, mPosition.y + pos.y, mPosition.z + pos.z}, JPH::Quat::sIdentity(), JPH::EActivation::Activate);
    }
}

void SphereCollider::HandleCollision(const JPH::BodyID& otherBodyID)
{
}

void SphereCollider::Scale(const SimpleMath::Vector3& scale)
{
    this->scale = scale;
    sphereShape->ScaleShape({this->scale.x, this->scale.y, this->scale.z});
}

void SphereCollider::RenderGUI()
{
    float pos[3] = {mPosition.x, mPosition.y, mPosition.z};
    float scale[3] = {this->scale.x, this->scale.y, this->scale.z};

    ImGui::DragFloat3("Position", pos, 0.1f);
    ImGui::DragFloat3("Scale", scale, 0.1f);

    SetPosition(SimpleMath::Vector3(pos[0], pos[1], pos[2]));
    Scale(SimpleMath::Vector3(scale[0], scale[1], scale[2]));
}

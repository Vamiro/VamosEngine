#include "BoxCollider.h"

#include "SimpleMath.h"
#include "Engine/Core/Object.h"

BoxCollider::BoxCollider(Object& parent, const SimpleMath::Vector3 extensions) : Component(parent, "BoxCollider"), boxShape(nullptr), extensions(extensions), physicsSystem(nullptr) {}

BoxCollider::~BoxCollider()
{
    // Удаляем сферическую форму и физическое тело при уничтожении компонента
    if (physicsSystem && !bodyID.IsInvalid())
    {
        physicsSystem->GetBodyInterface().DestroyBody(bodyID);
    }
    delete boxShape;
}

void BoxCollider::Initialize(JPH::PhysicsSystem& physicsSystem, const DirectX::SimpleMath::Vector3& initPosition)
{
    this->physicsSystem = &physicsSystem;

    mPosition = initPosition;
    auto pos = parent->transform->GetPositionVector();

    // Создаем сферическую форму
    boxShape = new JPH::BoxShape({extensions.x, extensions.y, extensions.z});

    // Настройки для создания тела с коллайдером
    JPH::BodyCreationSettings bodySettings(boxShape, {mPosition.x + pos.x, mPosition.y + pos.y, mPosition.z + pos.z}, JPH::Quat::sIdentity(), JPH::EMotionType::Static, PhysicsLayers::MOVING);

    // Создаем физическое тело и добавляем его в систему
    bodyID = physicsSystem.GetBodyInterface().CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

void BoxCollider::UpdatePosition() const
{
    // Обновляем позицию физического тела
    if (physicsSystem && !bodyID.IsInvalid())
    {
        auto pos = parent->transform->GetPositionVector();
        physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, {pos.x, pos.y, pos.z}, JPH::Quat::sIdentity(), JPH::EActivation::Activate);
    }
}

void BoxCollider::SetPosition(const SimpleMath::Vector3& position)
{
    mPosition = position;
    auto pos = parent->transform->GetPositionVector();

    // Обновляем позицию физического тела
    if (physicsSystem && !bodyID.IsInvalid())
    {
        physicsSystem->GetBodyInterface().SetPositionAndRotation(bodyID, {mPosition.x + pos.x, mPosition.y + pos.y, mPosition.z + pos.z}, JPH::Quat::sIdentity(), JPH::EActivation::Activate);
    }
}

void BoxCollider::HandleCollision(const JPH::BodyID& otherBodyID)
{
}

void BoxCollider::Scale(const SimpleMath::Vector3& scale) const
{
    boxShape->ScaleShape({scale.x, scale.y, scale.z});
}

void BoxCollider::RenderGUI()
{
}

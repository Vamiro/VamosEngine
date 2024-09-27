#include "BoxCollider.h"

#include "SimpleMath.h"
#include "Engine/Core/Object.h"

BoxCollider::BoxCollider(Object& parent, const SimpleMath::Vector3 extensions,
    JPH::PhysicsSystem& physicsSystem, const SimpleMath::Vector3& initPosition) :
        Component(parent, "BoxCollider"),
        mPosition(initPosition),
        boxShape(nullptr),
        extensions(extensions),
        physicsSystem(&physicsSystem)
{
}

BoxCollider::~BoxCollider()
{
    // Удаляем сферическую форму и физическое тело при уничтожении компонента
    if (physicsSystem && !bodyID.IsInvalid())
    {
        physicsSystem->GetBodyInterface().DestroyBody(bodyID);
    }
    delete boxShape;
}

void BoxCollider::Start()
{
    auto pos = parent->transform->GetPositionVector();

    // Создаем сферическую форму
    boxShape = new JPH::BoxShape({extensions.x, extensions.y, extensions.z});

    // Настройки для создания тела с коллайдером
    JPH::BodyCreationSettings bodySettings(boxShape, {mPosition.x + pos.x, mPosition.y + pos.y, mPosition.z + pos.z}, JPH::Quat::sIdentity(), JPH::EMotionType::Static, PhysicsLayers::MOVING);

    // Создаем физическое тело и добавляем его в систему
    bodyID = physicsSystem->GetBodyInterface().CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

void BoxCollider::Update()
{
    UpdatePosition();
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

void BoxCollider::Scale(const SimpleMath::Vector3& scale)
{
    this->scale = scale;
    boxShape->ScaleShape({this->scale.x, this->scale.y, this->scale.z});
}

void BoxCollider::RenderGUI()
{
    float pos[3] = {mPosition.x, mPosition.y, mPosition.z};
    float scale[3] = {this->scale.x, this->scale.y, this->scale.z};

    ImGui::DragFloat3("Position", pos, 0.1f);
    ImGui::DragFloat3("Scale", scale, 0.1f);

    SetPosition(SimpleMath::Vector3(pos[0], pos[1], pos[2]));
    Scale(SimpleMath::Vector3(scale[0], scale[1], scale[2]));
}

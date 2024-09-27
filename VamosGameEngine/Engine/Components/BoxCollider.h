#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <iostream>

#include "Component.h"
#include "Delegate.h"
#include "SimpleMath.h"
#include "Engine/Physics/PhysicsLayers.h"

class BoxCollider : public Component
{
public:
    explicit BoxCollider(Object& parent, DirectX::SimpleMath::Vector3 extensions, JPH::PhysicsSystem& physicsSystem, const DirectX::SimpleMath::Vector3& initPosition);
    ~BoxCollider() override;

    void Start() override;
    void Update() override;
    void RenderGUI() override;
    void UpdatePosition() const;
    void HandleCollision(const JPH::BodyID& otherBodyID);

    void SetPosition(const DirectX::SimpleMath::Vector3& position);
    void Scale(const DirectX::SimpleMath::Vector3& scale);
    [[nodiscard]] JPH::BodyID GetBodyID() const { return bodyID; }

private:
    DirectX::SimpleMath::Vector3 mPosition;
    JPH::BodyID bodyID;                                             // ID физического тела для этого коллайдера
    JPH::BoxShape* boxShape;                                        // Кубическая форма коллайдера
    DirectX::SimpleMath::Vector3 extensions;                        // Радиус сферы
    JPH::PhysicsSystem* physicsSystem;                              // Указатель на физическую систему для взаимодействия
    DirectX::SimpleMath::Vector3 scale;

    MulticastDelegate<> onCollision_;
};

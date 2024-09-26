#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/CollisionDispatch.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <iostream>

#include "Component.h"
#include "SimpleMath.h"
#include "Engine/Physics/PhysicsLayers.h"

class SphereCollider : public Component
{
public:
    explicit SphereCollider(Object& parent, float radius);
    ~SphereCollider();

    void RenderGUI() override;
    void Initialize(JPH::PhysicsSystem& physicsSystem, const DirectX::SimpleMath::Vector3& initPosition);
    void UpdatePosition() const;
    void SetPosition(const DirectX::SimpleMath::Vector3& position);
    void HandleCollision(const JPH::BodyID& otherBodyID);
    void Scale(const DirectX::SimpleMath::Vector3& scale);

private:
    DirectX::SimpleMath::Vector3 mPosition;
    JPH::BodyID bodyID;                 // ID физического тела для этого коллайдера
    JPH::SphereShape* sphereShape;      // Сферическая форма коллайдера
    float radius;                       // Радиус сферы
    JPH::PhysicsSystem* physicsSystem;  // Указатель на физическую систему для взаимодействия
    DirectX::SimpleMath::Vector3 scale;
};
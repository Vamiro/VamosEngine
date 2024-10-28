#include "BallComponent.h"
#include "ColliderComponent.h"
#include "FollowCamera.h"
#include "Engine/Core/Engine.h"

void BallComponent::Start()
{
    colliderComponent = parent->GetComponent<ColliderComponent>();
    followCamera = parent->GetComponent<FollowCamera>();

    Engine::Input().OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
         if(Engine::Input().IsKeyDown(InputKey::RightButton))
         {
             followCamera->RotateAround(-args.Offset.x * Engine::Input().MouseSensitivity);
         }
     });
}

void BallComponent::OnCollisionEnter(ColliderComponent* other)
{
    if (other->GetParent().name != "Floor" && other->GetParent().name != "Ball")
    {
        otherCollider = other;

    }
}

void BallComponent::Update(float deltaTime)
{
    if (parent->GetChildren().size() != items)
    {
        items = parent->GetChildren().size();
        colliderComponent->SetScale(colliderComponent->GetScale().x + 0.01f);
    }
    if (otherCollider != nullptr)
    {
        otherCollider->GetParent().SetParent(this->parent);
        otherCollider->GetParent().DeleteComponent(otherCollider);
        otherCollider = nullptr;
    }

    JPH::Vec3 velocity(0.0f, 0.0f, 0.0f);
    auto forward = followCamera->cameraTransform->GetForwardVector();
    forward.Normalize();
    forward *= ballSpeed;

    if (Engine::Input().IsKeyDown(InputKey::W))
    {
        velocity += JPH::Vec3(forward.x, 0.0f, forward.z);
    }
    if (Engine::Input().IsKeyDown(InputKey::S))
    {
        velocity -= JPH::Vec3(forward.x, 0.0f, forward.z);
    }

    auto right = followCamera->cameraTransform->GetRightVector();
    right.Normalize();
    right *= ballSpeed / 2.0f;

    if (Engine::Input().IsKeyDown(InputKey::A))
    {
        velocity += JPH::Vec3(right.x, 0.0f, right.z);
    }
    if (Engine::Input().IsKeyDown(InputKey::D))
    {
        velocity -= JPH::Vec3(right.x, 0.0f, right.z);
    }

    if (Engine::Input().IsKeyDown(InputKey::Space))
    {
        velocity += JPH::Vec3(0.0f, 0.001f, 0.0f);
    }

    if(velocity != JPH::Vec3(0.0f, 0.0f, 0.0f))
    {
        Engine::GetBodyInterface().SetLinearVelocity(
        colliderComponent->GetID(),
        {
            velocity.GetX(),
            Engine::GetBodyInterface().
            GetLinearVelocity(colliderComponent->GetID()).GetY()
            + velocity.GetY(),
            velocity.GetZ()
        });
    }

    if (parent->transform->GetGlobalPosition().y <= -100.0f)
    {
        Engine::GetBodyInterface().SetLinearVelocity(colliderComponent->GetID(), {0.0f, 0.0f, 0.0f});
        parent->transform->SetGlobalPosition({0.0f, 5.0f, 0.0f});
    }
}

void BallComponent::RenderGUI()
{
    ImGuiHelper::FloatGUI("Speed", ballSpeed);
}

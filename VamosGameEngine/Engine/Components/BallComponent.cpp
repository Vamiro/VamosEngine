#include "BallComponent.h"
#include "ColliderComponent.h"
#include "FollowCamera.h"
#include "Engine/Core/GameEngine.h"

void BallComponent::Start()
{
    colliderComponent = parent->GetComponent<ColliderComponent>();
    auto collider = parent->GetComponent<ColliderComponent>();
    if (collider)
    {
        colliderComponent->OnCollision().BindLambda([this](ColliderComponent* other) {
            if(other->GetParent().name != "Floor" && other->GetParent().name != "Ball")
            {
                otherCollider = other;
            }
        });
    }

    followCamera = parent->GetComponent<FollowCamera>();

    GameEngine::Input().OnMouseMove().AddLambda([&](const InputDevice::MouseMoveEventArgs& args) {
         if(GameEngine::Input().IsKeyDown(InputKey::RightButton))
         {
             followCamera->RotateAround(-args.Offset.x * GameEngine::Input().MouseSensitivity);
         }
     });
}

void BallComponent::Update(float deltaTime)
{
    if (parent->GetChildren().size() != items)
    {
        items = parent->GetChildren().size();
        auto size = items / 50.0f + 1.0f;
        colliderComponent->SetScale(size);
        //parent->transform->SetScale({size,size,size});
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

    if (GameEngine::Input().IsKeyDown(InputKey::W))
    {
        velocity += JPH::Vec3(forward.x, 0.0f, forward.z);
    }
    if (GameEngine::Input().IsKeyDown(InputKey::S))
    {
        velocity -= JPH::Vec3(forward.x, 0.0f, forward.z);
    }

    auto right = followCamera->cameraTransform->GetRightVector();
    right.Normalize();
    right *= ballSpeed / 2.0f;

    if (GameEngine::Input().IsKeyDown(InputKey::A))
    {
        velocity += JPH::Vec3(right.x, 0.0f, right.z);
    }
    if (GameEngine::Input().IsKeyDown(InputKey::D))
    {
        velocity -= JPH::Vec3(right.x, 0.0f, right.z);
    }

    if (GameEngine::Input().IsKeyDown(InputKey::Space))
    {
        velocity += JPH::Vec3(0.0f, 0.001f, 0.0f);
    }

    if(velocity != JPH::Vec3(0.0f, 0.0f, 0.0f))
    {
        GameEngine::GetBodyInterface().SetLinearVelocity(
        colliderComponent->GetID(),
        {
            velocity.GetX(),
            GameEngine::GetBodyInterface().
            GetLinearVelocity(colliderComponent->GetID()).GetY()
            + velocity.GetY(),
            velocity.GetZ()
        });
    }
}

void BallComponent::RenderGUI()
{
    ImGuiHelper::FloatGUI("Speed", ballSpeed);
}

#include "FollowCamera.h"
#include "Transform.h"
#include "Engine/Core/GameObject.h"

FollowCamera::FollowCamera(GameObject& parent, GameObject& target):
    Component(parent, "FollowCamera"),
    target(&target)
{
}

void FollowCamera::Start()
{
    parent->transform->SetPosition(
        target->transform->GetGlobalPosition() +
        SimpleMath::Vector3(0.0f, 10.0f, -10.0f)
    );
    parent->transform->SetLookAtPos(target->transform->GetGlobalPosition());
}

void FollowCamera::Update(float deltaTime)
{
    auto pos = target->transform->GetGlobalPosition();
    parent->transform->KeepDistance(SimpleMath::Vector3(pos.x, 30.0f, pos.z), 30.0f);
    parent->transform->SetLookAtPos(target->transform->GetGlobalPosition());
}

void FollowCamera::RenderGUI()
{
}

void FollowCamera::Destroy()
{
}

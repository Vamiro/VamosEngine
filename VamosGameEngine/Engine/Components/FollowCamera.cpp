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
        target->transform->GetPositionVector() +
        SimpleMath::Vector3(0.0f, 10.0f, -10.0f)
    );
    parent->transform->SetLookAtPos(target->transform->GetPositionVector());
}

void FollowCamera::Update()
{
    auto pos = target->transform->GetPositionVector();
    parent->transform->KeepDistance(SimpleMath::Vector3(pos.x, 30.0f, pos.z), 30.0f);
    parent->transform->SetLookAtPos(target->transform->GetPositionVector());
}

void FollowCamera::RenderGUI()
{
}

void FollowCamera::Destroy()
{
}

#include "FollowCamera.h"
#include "Engine/Utilities/ImGuiHelper.h"

FollowCamera::FollowCamera(GameObject& parent, Transform& cameraTransform) :
    Component(parent, "FollowCamera"),
    cameraTransform(&cameraTransform)
{
}

void FollowCamera::Start()
{
}

void FollowCamera::Update(float deltaTime)
{
    Follow();
}

void FollowCamera::RotateAround(float mouseX)
{
    if (cameraTransform != nullptr)
    {
        cameraTransform->RotateAround(parent->transform->GetGlobalPosition(), DirectX::SimpleMath::Vector3::Up, mouseX);
        cameraOffset = DirectX::SimpleMath::Vector3::Transform(cameraOffset,
                                                      DirectX::SimpleMath::Matrix::CreateFromAxisAngle(
                                                          DirectX::SimpleMath::Vector3::Up, DirectX::XMConvertToRadians(mouseX)));
    }
}

void FollowCamera::Follow()
{
    if (cameraOffset == DirectX::SimpleMath::Vector3::Zero)
    {
        cameraOffset.z = -distance;
        cameraOffset.y = height;
    }

    //cameraTransform.position = Vector3.Lerp(cameraTransform.position, this.transform.position + cameraOffset, smoothSpeed * Time.deltaTime);
    cameraTransform->SetPosition(parent->transform->GetGlobalPosition() + cameraOffset);
    cameraTransform->SetLookAtPos(parent->transform->GetGlobalPosition() + centerOffset);
}

void FollowCamera::RenderGUI()
{
    ImGuiHelper::Vector3GUI("Camera Offset", cameraOffset);
}

void FollowCamera::Destroy()
{
}

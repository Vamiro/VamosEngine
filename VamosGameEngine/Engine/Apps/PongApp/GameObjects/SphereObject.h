#pragma once
#include "Engine/Components/Model.h"
#include "Engine/Components/SphereCollider.h"
#include "../../../Core/Object.h"

#include "Engine/Core/GameObject.h"

class SphereObject : public GameObject {
public:
    SphereObject(const std::string& name);

    void Destroy() override;

    void Update(float deltaTime) override;
    void Render(const SimpleMath::Matrix& viewProjectionMatrix) override;

private:
    SphereCollider* sphereCollider;
    Model* model;
};

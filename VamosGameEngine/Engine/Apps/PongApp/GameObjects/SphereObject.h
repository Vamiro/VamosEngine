#pragma once
#include "Engine/Rendering/GameObject.h"


class SphereObject : public GameObject {
public:
    explicit SphereObject(const std::string& name);
    void Update() override;

    [[nodiscard]] BoundingSphere GetBoundingSphere() const { return boundingSphere; }
    void SetBoundingSphereRadius(const float radius) { boundingSphere.Radius = radius; }

private:
    BoundingSphere boundingSphere;
};

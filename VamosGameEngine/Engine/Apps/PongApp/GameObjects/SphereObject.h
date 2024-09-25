#pragma once
#include "Engine/Components/Model.h"
#include "Engine/Rendering/GameObject.h"


class SphereObject : public GameObject {
public:
    explicit SphereObject(const std::string& name);

    [[nodiscard]] BoundingSphere GetBoundingSphere() const { return boundingSphere; }
    void SetBoundingCenter(XMFLOAT3 value) { boundingSphere.Center = value; }
    void SetBoundingSphereRadius(const float radius) { boundingSphere.Radius = radius; }


private:
    BoundingSphere* boundingSphere;
    Model* Model;
    Transform* Transform;
};

